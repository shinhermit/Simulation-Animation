///////////////////////////////////////////////////////////////////////////////
//  File         :    wlMesh.cxx                                             //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Classe implementant une mesh avec gestion des          //
//                    relations topologiques, insertions/suppressions, etc   //
//  Related file :    wlMesh.h                                               //
//                    Wilfrid Lefer - UPPA - 2007                            //
///////////////////////////////////////////////////////////////////////////////


#include "wlMesh.h"
#include <float.h>
#include <QFile>

// mesh par defaut : tetrahedre
float srefs[][5] = { {0,0,0,0.5,0}, {10,0,0,1,0.5}, {5,8.66,0,0.5,1}, {5,1.59,7.07,0,0.5} };
unsigned int arefs[][2] = { {0,1}, {1,2}, {2,0}, {0,3}, {1,3}, {2,3} };
unsigned int frefs[][3] = { {0,2,1}, {0,1,3}, {1,2,3}, {2,0,3} };
unsigned int nsrefs=4, narefs=6, nfrefs=4;

// Quelques variable globales (eh oui c'est pas genial mais je ne pouvais faire autrement
// a moins de faire (tres) complique !
static QVector<float> origine;            // le sommet d'origine pour la deformation
static QVector<float> vecteur;            // le vecteur deplacement du sommet d'origine
static unsigned int Dmax, Dcour;          // la taille de la zone a deformer en nombre d'aretes depuis sommet courant
static float dmax;                        // la taille de la zone a deformer en distance euclidienne
static QVector<unsigned int> Cmarque;     // un tableau pour marquer les coins parcourus
static QVector<unsigned int> Smarque;     // un tableau pour marquer les sommets parcourus
static QVector<unsigned int> Tmarque;     // un tableau pour marquer les triangles parcourus
static QVector<QVector<float> > Tnormals; // un tableau pour stocker les normales aux triangles

// cette macro passe les eventuelles lignes blanches
// si <comments> vaut "true", elle passe egalement les lignes de commentaires
#define SKIP_LINES(comments) { \
  while (!f.atEnd()) { \
    ba = f.readLine(); \
    ba = ba.trimmed(); \
    if (ba.isEmpty()) \
      continue; \
    if(ba[0] == '#') { \
      this->Trace("%s", ba.constData()); \
      if (!comments) { \
        this->Error("erreur de syntaxe dans le fichier \"%s\" : donnees incompletes pour le champ en cours de lecture", this->GetFilename().toAscii().constData()); \
        this->Trace("<- LoadMesh()"); \
        return; \
      } \
      continue; \
    } \
    break; \
  } \
}

// cette macro teste si on a atteint la fin du fichier et genere une erreur si tel est le cas
// attention : cela ne marche que si elle est appelee apres la sequence d'instructions suivantes :
//   ba = f.readLine();
//   ba = ba.trimmed();
// cette sequence figure dans la macro SKIP_LINES(), donc on appelle generalement les 2 macros a la suite
#define END_OF_FILE_TEST() { \
  if (ba.isEmpty()) { \
    this->Error("erreur de syntaxe dans le fichier \"%s\" : fichier incomplet", this->GetFilename().toAscii().constData()); \
    this->Trace("<- LoadMesh()"); \
    return; \
  } \
}

////////////////////////////
// La creation de la mesh //
////////////////////////////

wlMesh::wlMesh(int debug, wlQGLViewer *v, QString fname, QString tname)
  : wlCore(debug)
{
  if (fname.isEmpty())
    this->Trace("-> wlMesh()");
  else if (tname.isEmpty())
    this->Trace("-> wlMesh(\"%s\")", fname.toAscii().constData());
  else
    this->Trace("-> wlMesh(\"%s\", \"%s\")", fname.toAscii().constData(), tname.toAscii().constData());
  this->Clear();
  this->viewer = NULL;
  this->fname = fname;
  this->tname = tname;
  if (fname.isEmpty()) {
    for (unsigned int i=0 ; i<nsrefs ; i++)
      this->G << wlSommet(this, srefs[i][0], srefs[i][1], srefs[i][2], srefs[i][3], srefs[i][4]);
    for (unsigned int i=0 ; i<nfrefs ; i++) {
      this->T << wlTriangle(this);
      this->S << frefs[i][0] << frefs[i][1] << frefs[i][2];
      this->C << wlCoin(this);
      this->C << wlCoin(this);
      this->C << wlCoin(this);
    }
    this->nb_infinite_coins = this->nb_infinite_sommets = this->nb_infinite_triangles = 0;
    // contruction de la table O
    this->BuildOTable();
  } else
    this->LoadMesh(fname);
  if (!tname.isEmpty())
    this->LoadTexture(tname);
  this->ConnectViewer(v);
  this->pshader = new QGLShaderProgram;
  this->Trace("<- wlMesh()");
}

wlMesh::wlMesh(wlMesh *mesh)
  : wlCore(mesh->GetDebug())
{
  this->Trace("-> wlMesh(%p)", mesh);
  this->Clear();
  this->fname = QString();
  this->tname = QString();
  // copie des sommets, coins et triangles
  this->G = mesh->GetSommets();
  this->C = mesh->GetCoins();
  for (unsigned int i=0 ; i<this->GetNumberOfCoins() ; i++)
    this->S << this->C[i].s().GetId();
  this->T = mesh->GetTriangles();
  this->nb_infinite_coins = mesh->GetNumberOfInfiniteCoins();
  this->nb_infinite_sommets = mesh->HasInfiniteSommet() > 0;
  this->nb_infinite_triangles = mesh->GetNumberOfInfiniteTriangles();
  // recontruction de la table O
  this->BuildOTable();
  // association du viewer
  this->SetViewer(mesh->GetViewer());
  this->Trace("<- wlMesh()");
}

void
wlMesh::InitMesh(std::vector<std::vector<float> > vertices,
                 std::vector<std::vector<int> > triangles,
		 unsigned int HasInfiniteVertex,
		 unsigned int NbInfiniteTriangles)
{
  this->Trace("-> InitMesh(%d, %d)", vertices.size(), triangles.size());
  this->fname = QString();
  this->tname = QString();
  this->Clear();
  for (std::vector<std::vector<float> >::iterator it=vertices.begin() ; it!=vertices.end() ; it++)
    this->G << wlSommet(this, *it);
  for (std::vector<std::vector<int> >::iterator it=triangles.begin() ; it!=triangles.end() ; it++) {
    this->T << wlTriangle(this);
    this->S << (*it)[0] << (*it)[1] << (*it)[2];
    this->C << wlCoin(this);
    this->C << wlCoin(this);
    this->C << wlCoin(this);
  }
  this->nb_infinite_sommets = HasInfiniteVertex;
  this->nb_infinite_triangles = NbInfiniteTriangles;
  this->nb_infinite_coins = NbInfiniteTriangles * 3;
  // contruction de la table O
  this->BuildOTable();
  this->makeList(1);
  fill(this->size.begin(), this->size.end(), 0);
  emit numberOfTrianglesHasChanged(this->GetNumberOfFiniteTriangles());
  this->Trace("<- InitMesh()");
}

void
wlMesh::LoadMesh(QString filename)
{
  QString fname(this->fname);
  if (!filename.isEmpty())
    fname = filename;
  if (fname.isEmpty())
    return;
  this->Trace("-> LoadMesh(\"%s\")", fname.toAscii().constData());
  QFile f(fname);
  // on tente d'ouvrir le fichier
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    this->Error("impossible d'ouvrir le fichier \"%s\"", filename.toAscii().constData());
    this->Trace("<- LoadMesh()");
    return;
  }
  QByteArray ba;
  // on passe les eventuelles lignes blanches et lignes de commentaires
  SKIP_LINES(true);
  END_OF_FILE_TEST();
  // on lit la premiere ligne, le format reconnu pour l'instant etant "[C] [N] OFF"
  QString s(ba);
  QStringList l = s.split(QRegExp("\\s+"), QString::SkipEmptyParts);
  f.close();
  if (l.count() <= 3 && (l.last() == "OFF" || l.last() == "COFF" || l.last() == "NOFF"))
    this->LoadOFFFile(fname);
  else {
    this->Error("type de fichier non reconnu");
    this->Trace("<- LoadMesh()");
    return;
  }
  this->Trace("<- LoadMesh()");
}

void
wlMesh::LoadOFFFile(QString filename)
{
  this->Trace("-> LoadOFFFile(\"%s\")", filename.toAscii().constData());
  QFile f(filename);
  // on ouvre le fichier
  f.open(QIODevice::ReadOnly | QIODevice::Text);
  QByteArray ba;
  // on passe les eventuelles lignes blanches et lignes de commentaires
  SKIP_LINES(true);
  END_OF_FILE_TEST();
  // on lit la premiere ligne, le format reconnu pour l'instant etant "[C] [N] OFF"
  QString s(ba);
  QStringList l = s.split(QRegExp("\\s+"), QString::SkipEmptyParts);
  QString ll(l.first());
  int N = 3;
  for (; ll != "OFF" ; ll.remove(0, 1)) {
    if (ll[0] == 'C')
      N += 4;
    else if (ll[0] == 'N')
      N += 3;
    else {
      this->Error("erreur de syntaxe : 1ere ligne (le format supporte actuellement est \"[C][N]OFF\"");
      this->Trace("<- LoadOFFFile()");
      return;
    }
  }
  // on passe les eventuelles lignes blanches et lignes de commentaires
  SKIP_LINES(true);
  END_OF_FILE_TEST();
  // on lit la deuxieme ligne, qui pour l'instant doit contenir 3 entiers (format OFF)
  s = QString(ba);
  l = s.split(QRegExp("\\s+"), QString::SkipEmptyParts);
  if (l.count() != 3) {
    this->Error("erreur de syntaxe : 2e ligne : nombre d'arguments incorrect (%d)", l.count());
    this->Trace("<- LoadMesh()");
    return;
  }
  bool ok;
  int vcount = l[0].toInt(&ok);
  if (!ok || vcount<3) {
    this->Error("erreur de syntaxe : 2e ligne : nombre de sommets incorrect (\"%s\")", l[0].toAscii().constData());
    this->Trace("<- LoadMesh()");
    return;
  }
  int fcount = l[1].toInt(&ok);
  if (!ok || fcount<1) {
    this->Error("erreur de syntaxe : 2e ligne : nombre de faces incorrect (\"%s\")", l[1].toAscii().constData());
    this->Trace("<- LoadMesh()");
    return;
  }
  // lecture des sommets
  std::vector<std::vector<float> > vertices;
  do {
    // on passe les eventuelles lignes blanches et lignes de commentaires
    SKIP_LINES(true);
    END_OF_FILE_TEST();
    // on lit un sommet
    s = QString(ba);
    l = s.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if (l.count() != N) {
      this->Error("erreur de syntaxe : %d valeurs pour un sommet (\"%s\") contre %d attendues", l.count(), s.toAscii().constData(), N);
      this->Trace("<- LoadMesh()");
      return;
    }
    // wlSommet n'admettant pas des attributs de couleur, s'ils sont presents on les enleve
    if (N == 10)
      l.erase(l.begin()+3, l.begin()+7);
    std::vector<float> vertex;
    for (; !l.isEmpty() ; l.pop_front()) {
      vertex.push_back(l.first().toFloat(&ok));
      if (!ok) {
	this->Error("erreur de syntaxe : valeur flottante invalide (\"%s\")", l.first().toAscii().constData());
	this->Trace("<- LoadMesh()");
	return;
      }
    }
    vertices.push_back(vertex);
  } while ((int)vertices.size() < vcount);
  // lecture des faces
  std::vector<std::vector<int> > faces;
  std::vector<int> face(3);
  do {
    // on passe les eventuelles lignes blanches et lignes de commentaires
    SKIP_LINES(true);
    END_OF_FILE_TEST();
    // on lit une face
    s = QString(ba);
    l = s.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    // on ne supporte que les triangles
    if (l.count() != 4 && l.count() != 8) {
      this->Error("erreur de syntaxe : %d valeurs pour une face (\"%s\")", s.toAscii().constData());
      this->Trace("<- LoadMesh()");
      return;
    }
    for (int i=0 ; i<3 ; i++) {
      face[i] = l[i+1].toInt(&ok);
      if (!ok) {
	this->Error("erreur de syntaxe : valeur entiere invalide (\"%s\")", l[i+1].toAscii().constData());
	this->Trace("<- LoadMesh()");
	return;
      }
      // on verifie la validite de l'index
      if (face[i] < 0 || face[i] >= (int)vertices.size()) {
	this->Error("erreur : indice de sommet invalide (%d)", face[i]);
	this->Trace("<- LoadMesh()");
	return;
      }
    }
    faces.push_back(face);
  } while ((int)faces.size() < fcount);
  // on verifie que l'on a bien lu tout le fichier
  ba.clear();
  SKIP_LINES(true);
  if (!ba.isEmpty()) {
    this->Error("erreur de syntaxe : tout a ete lu mais des donnees subsistent");
    this->Trace("<- LoadMesh()");
    return;
  }
  f.close();
  this->fname = filename;
  // on cree la mesh
  this->InitMesh(vertices, faces);
  this->Trace("<- LoadOFFFile()");
  return;
}

void
wlMesh::LoadTexture(QString filename)
{
  this->Trace("-> LoadTexture(\"%s\")", filename.toAscii().constData());
  QImage image;
  if (!image.load(filename))
    this->Error("impossible de lire une image dans le fichier \"%s\"", filename.toAscii().constData());
  this->SetTexture(image);
  this->Trace("<- LoadTexture()");
}

void
wlMesh::SetTexture(QImage image)
{
  this->Trace("-> SetTexture(%ux%u)", image.width(), image.height());
  image.convertToFormat(QImage::Format_ARGB32);
  // on est oblige de faire cette conversion car Qt ne propose que le codage ARGB quand OpenGL ne propose que RGBA !
  this->w = image.width();
  this->h = image.height();
  this->texture = QVector<unsigned int>(this->w*this->h);
  QRgb *pixels = (QRgb *)image.bits();
  for (int i=0 ; i<this->texture.size() ; i++)
    this->texture[i] = (pixels[i] & 0xff000000) + ((pixels[i] & 0x00ff0000) >> 16) + (pixels[i] & 0x0000ff00) + ((pixels[i] & 0x000000ff) << 16);
  this->Modified("DisplayList");
  this->Trace("<- SetTexture()");
}

class triplet
{
public:
  triplet(unsigned int min, unsigned int max, unsigned int c) {this->min=min; this->max=max; this->c=c;};
  bool operator < (const triplet &t) const {
    return this->min < t.min ? 1 :
           this->min > t.min ? 0 :
           this->max < t.max ? 1 :
           this->max > t.max ? 0 :
           this->c < t.c ? 1 :
           this->c > t.c ? 0 :
           0;
  };
  unsigned int min, max, c;
};

void
wlMesh::BuildOTable()
{
  std::list<triplet> t;
  // pour tous les coins de la mesh on construit la liste des triplets :
  //  <indice de celui des 2 autres sommets du triangle d'indice le plus petit>
  //  <indice de celui des 2 autres sommets du triangle d'indice le plus grand>
  //  <indice du sommet courant>
  // que l'on range dans la table t
  for (int i=this->C.count()-1 ; i>=0 ; i--) {
    // a et b contiennent les indices des 2 autres sommets du triangle
    unsigned int a=this->C[i].n().s().GetId(), b=this->C[i].n().n().s().GetId();
    // on range ces indices en ordre croissant dans la table t
    t.push_back(triplet(a<b ? a : b, a<b ? b : a, this->C[i].GetId()));
  }
  // on trie la table t selon l'ordre croissant des triplets
  t.sort();
  // on alloue la table O
  this->O.resize(this->C.size());
  // pour chaque coin c : O[c] contient le coin oppose par rapport a l'arete opposee
  for (std::list<triplet>::const_iterator it=t.begin() ; it!=t.end() ;) {
    unsigned int a=(*it++).c, b=(*it++).c;
    this->O[a] = b;
    this->O[b] = a;
  }
}

void
CalculerNormaleTriangle(wlTriangle &t)
{
  QVector<unsigned int>::const_iterator idx = t.GetIndexIterator();
  QVector<float> V0(t.GetMesh()->GetSommet(idx[0])), V1(t.GetMesh()->GetSommet(idx[1])), V2(t.GetMesh()->GetSommet(idx[2]));
  V1[0] -= V0[0]; V1[1] -= V0[1]; V1[2] -= V0[2];
  V2[0] -= V0[0]; V2[1] -= V0[1]; V2[2] -= V0[2];
  V0[0] = V1[1] * V2[2] - V1[2] * V2[1];
  V0[1] = V1[2] * V2[0] - V1[0] * V2[2];
  V0[2] = V1[0] * V2[1] - V1[1] * V2[0];
  float x = (float)sqrt((double)(V0[0] * V0[0] + V0[1] * V0[1] + V0[2] * V0[2]));
  V0[0] /= x; V0[1] /= x; V0[2] /= x;
  Tnormals[t.GetId()] = V0;
}

void
AjouterContributionNormale(wlCoin &c)
{
  c.s().AddNormal(Tnormals[c.t().GetId()]);
}

void
Normalise(wlSommet &s)
{
  s.Normalize();
}

void
wlMesh::ComputeNormals()
{
  this->Trace("-> ComputeNormals()");
  Tnormals.resize(this->GetNumberOfFiniteTriangles());
  Tnormals.fill(QVector<float>(3, 0));
  this->TparcoursFinis(CalculerNormaleTriangle);
  this->CparcoursFinis(AjouterContributionNormale);
  this->SparcoursFinis(Normalise);
  Tnormals.clear();
  this->Modified("DisplayList");
  this->Trace("<- ComputeNormals()");
}

void
wlMesh::Clear()
{
  this->G.clear();
  this->T.clear();
  this->C.clear();
  this->S.clear();
  this->O.clear();
  this->A.clear();
  this->liste =  this->gltexname = UINT_MAX;
  this->fname = this->tname = QString();
  this->size = std::vector<float>(3);
  this->center = std::vector<float>(3);
  this->extends = std::vector<float>(6);
  fill(this->size.begin(), this->size.end(), 0);
  fill(this->center.begin(), this->center.end(), 0);
  fill(this->extends.begin(), this->extends.end(), 0);
  this->Modified("DisplayList");
  this->Modified("MeshSize");
}

void
wlMesh::SaveMeshAsOFF(QString filename)
{
  this->Trace("-> SaveMeshAsOFF(\"%s\")", filename.toAscii().constData());
  QFile f(filename);
  // on tente d'ouvrir le fichier
  if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
    this->Error("impossible de creer le fichier \"%s\"", filename.toAscii().constData());
    this->Trace("<- SaveMeshAsOFF()");
    return;
  }
  if (f.write(QString("# Fichier genere par la classe wlMesh\nOFF\n%1 %2 0# %1 sommets\n").arg(QString::number(this->GetNumberOfFiniteSommets())).arg(QString::number(this->GetNumberOfFiniteTriangles())).arg(QString::number(this->GetNumberOfFiniteSommets())).toAscii()) == -1) {  
    this->Error("erreur lors de l'ecriture du fichier \"%s\"", filename.toAscii().constData());
    this->Trace("<- SaveMeshAsOFF()");
    return;
  }
  if (this->G[0].HasNormal()) {
    unsigned int n = this->G[0].HasTextureCoordinates() ? 5 : 3;
    for (unsigned int i=0 ; i<this->GetNumberOfFiniteSommets() ; i++)
      if (f.write(QString("%1 %2 %3 %4 %5 %6\n").arg(QString::number(this->G[i][0])).arg(QString::number(this->G[i][1])).arg(QString::number(this->G[i][2])).arg(QString::number(this->G[i][n])).arg(QString::number(this->G[i][n+1])).arg(QString::number(this->G[i][n+2])).toAscii()) == -1) {  
	this->Error("erreur lors de l'ecriture du fichier \"%s\"", filename.toAscii().constData());
	this->Trace("<- SaveMeshAsOFF()");
	return;
      }
  } else {
    for (unsigned int i=0 ; i<this->GetNumberOfFiniteSommets() ; i++)
      if (f.write(QString("%1 %2 %3\n").arg(QString::number(this->G[i][0])).arg(QString::number(this->G[i][1])).arg(QString::number(this->G[i][2])).toAscii()) == -1) {  
	this->Error("erreur lors de l'ecriture du fichier \"%s\"", filename.toAscii().constData());
	this->Trace("<- SaveMeshAsOFF()");
	return;
      }
  }
  if (f.write(QString("# %1 triangles\n").arg(QString::number(this->GetNumberOfFiniteTriangles())).toAscii()) == -1) {  
    this->Error("erreur lors de l'ecriture du fichier \"%s\"", filename.toAscii().constData());
    this->Trace("<- SaveMeshAsOFF()");
    return;
  }
  for (unsigned int i=0 ; i<this->GetNumberOfFiniteTriangles() ; i++)
    if (f.write(QString("3 %1 %2 %3\n").arg(QString::number(this->S[i*3])).arg(QString::number(this->S[i*3+1])).arg(QString::number(this->S[i*3+2])).toAscii()) == -1) {  
      this->Error("erreur lors de l'ecriture du fichier \"%s\"", filename.toAscii().constData());
      this->Trace("<- SaveMeshAsOFF()");
      return;
    }
  f.close();
  this->Trace("<- SaveMeshAsOFF()");
}

////////////////////////////
// L'affichage du contenu //
////////////////////////////

void
wlMesh::PrintSelf()
{
  this->Print("%d sommets  %d triangles  %d coins", this->G.count(), this->T.count(), this->C.count());
  if (this->HasInfiniteSommet())
    this->Print("  dont 1 sommet infini  %d aretes infinies  %d triangles infinis",
	        this->GetNumberOfInfiniteAretes(), this->GetNumberOfInfiniteTriangles());
  if (this->S.count() > 0) {
    this->Print("taille : (%.2f, %.2f, %.2f)", this->GetSize()[0], this->GetSize()[1], this->GetSize()[2]);
    this->Print("centre : (%.2f, %.2f, %.2f)", this->GetCenter()[0], this->GetCenter()[1], this->GetCenter()[2]);
    this->Print("limites : ((%.2f, %.2f), (%.2f, %.2f), (%.2f, %.2f))",
		this->GetExtends()[0], this->GetExtends()[1], this->GetExtends()[2],
		this->GetExtends()[3], this->GetExtends()[4], this->GetExtends()[5]);
    this->Print("rayon : %.2f", this->GetRadius());
  }
}

void
wlMesh::PrintContent()
{
  this->PrintSelf();
  if (S.count() > 0) {
    this->Print("Sommets :\n");
    for (int i=0 ; i<this->G.count() ; i++)
      this->G[i].PrintSelf();
    this->Print("Triangles :\n");
    for (int i=0 ; i<this->T.count() ; i++)
      this->T[i].PrintSelf();
    this->Print("Coins, leur sommet associe, le triangle les contenant, leur coin oppose, leurs 2 suivants et leurs coins droit et gauche :\n");
    for (int i=0 ; i<this->C.count() ; i++)
      this->Print("  %d %d %d %d %d %d %d %d", this->C[i].GetId(),
		  this->C[i].s().GetId(), this->C[i].t().GetId(),
		  this->C[i].o().GetId(),
		  this->C[i].n().GetId(), this->C[i].n().n().GetId(),
		  this->C[i].r().GetId(), this->C[i].l().GetId());
    this->Print("Table S :\n");
    for (int i=0 ; i<this->S.count() ; i++)
      this->Print("  %u %u", i, this->S[i]);
    this->Print("Table O :\n");
    for (int i=0 ; i<this->O.count() ; i++)
      this->Print("  %u %u", i, this->O[i]);
  }
}

//////////////////////////////////
// L'acces aux donnees globales //
//////////////////////////////////

std::vector<float>
wlMesh::GetSize()
{
  if (!this->UpToDate("MeshSize")) {
    // on recalcule la taille de la mesh
    this->extends[0] = this->extends[2] = this->extends[4] = FLT_MAX;
    this->extends[1] = this->extends[3] = this->extends[5] = -FLT_MAX;
    for (int i=0 ; i<(int)this->GetNumberOfFiniteSommets() ; i++) {
      this->extends[0] = this->G[i][0] < this->extends[0] ? this->G[i][0] : this->extends[0];
      this->extends[1] = this->G[i][0] > this->extends[1] ? this->G[i][0] : this->extends[1];
      this->extends[2] = this->G[i][1] < this->extends[2] ? this->G[i][1] : this->extends[2];
      this->extends[3] = this->G[i][1] > this->extends[3] ? this->G[i][1] : this->extends[3];
      this->extends[4] = this->G[i][2] < this->extends[4] ? this->G[i][2] : this->extends[4];
      this->extends[5] = this->G[i][2] > this->extends[5] ? this->G[i][2] : this->extends[5];
    }
    this->size[0] = this->extends[1] - this->extends[0];
    this->size[1] = this->extends[3] - this->extends[2];
    this->size[2] = this->extends[5] - this->extends[4];
    this->center[0] = this->extends[0] + this->size[0] / 2;
    this->center[1] = this->extends[2] + this->size[1] / 2;
    this->center[2] = this->extends[4] + this->size[2] / 2;
    this->radius = (float)sqrt((double)(this->size[0] * this->size[0] + this->size[1] * this->size[1] + this->size[2] * this->size[2])) / 2;
    this->Updated("MeshSize");
  }
  return this->size;
}

std::vector<float>
wlMesh::GetExtends()
{
  // cet appel a GetSize() a pour objectif de verifier que la donnee est a jour
  (void)this->GetSize();
  return this->extends;
}

std::vector<float>
wlMesh::GetCenter()
{
  // cet appel a GetSize() a pour objectif de verifier que la donnee est a jour
  (void)this->GetSize();
  return this->center;
}

float
wlMesh::GetRadius()
{
  // cet appel a GetSize() a pour objectif de verifier que la donnee est a jour
  (void)this->GetSize();
  return this->radius;
}

///////////////////////
// L'acces aux coins //
///////////////////////

bool
wlMesh::IsInfinite(wlCoin &c)
{
  return c.GetId() >= this->GetNumberOfFiniteCoins();
}

QVector<wlCoin>
wlMesh::GetFiniteCoins()
{
  QVector<wlCoin> l(this->C);
  l.erase(l.begin()+this->GetNumberOfFiniteCoins(), l.end());
  return l;
}

void
wlMesh::Cparcours(void (*appliquer)(wlCoin &))
{
  for (unsigned int i=0 ; i<this->GetNumberOfCoins() ; i++)
    (*appliquer)(this->C[i]);
}

void
wlMesh::CparcoursFinis(void (*appliquer)(wlCoin &))
{
  for (unsigned int i=0 ; i<this->GetNumberOfFiniteCoins() ; i++)
    (*appliquer)(this->C[i]);
}

void
wlMesh::Cparcours(wlCoin &c0, void (*appliquer)(wlCoin &))
{
  Tmarque.fill(1, this->GetNumberOfTriangles());
  this->Cparcours_rec(c0, appliquer);
  Tmarque.clear();
}

void
wlMesh::CparcoursFinis(wlCoin &c0, void (*appliquer)(wlCoin &))
{
  Tmarque.fill(1, this->GetNumberOfTriangles());
  this->Cparcours_rec(c0, appliquer, 1);
  Tmarque.clear();
}

void
wlMesh::Cparcours_rec(wlCoin &c, void (*appliquer)(wlCoin &), int finite)
{
  if (!finite || !this->IsInfinite(c)) {
    (*appliquer)(c);
    (*appliquer)(c.n());
    (*appliquer)(c.n().n());
  }
  Tmarque[c.t().GetId()] = 0;
  if (Tmarque[c.r().t().GetId()])
    this->Cparcours_rec(c.r(), appliquer, finite);
  if (Tmarque[c.l().t().GetId()])
    this->Cparcours_rec(c.l(), appliquer, finite);
}

/////////////////////////
// L'acces aux sommets //
/////////////////////////

bool
wlMesh::IsInfinite(wlSommet &s)
{
  return s.GetId() >= this->GetNumberOfFiniteSommets();
}

QVector<wlSommet>
wlMesh::GetFiniteSommets()
{
  QVector<wlSommet> l(this->G);
  l.erase(l.begin()+this->GetNumberOfFiniteSommets(), l.end());
  return l;
}

std::vector<std::vector<float> >
wlMesh::GetStdSommets()
{
  std::vector<std::vector<float> > verts;
  if (this->G.isEmpty())
    return verts;
  for (QVector<wlSommet>::const_iterator it = this->G.begin() ; it != this->G.end() ; it++) {
    std::vector<float> v = (*it).toStdVector();
    verts.push_back(v);
  }
  return verts;
}

std::vector<std::vector<float> >
wlMesh::GetStdFiniteSommets()
{
  std::vector<std::vector<float> > verts = this->GetStdSommets();
  if (this->HasInfiniteSommet())
    verts.pop_back();
  return verts;
}

void
wlMesh::Sparcours(void (*appliquer)(wlSommet &))
{
  for (unsigned int i=0 ; i<this->GetNumberOfSommets() ; i++)
    (*appliquer)(this->G[i]);
}

void
wlMesh::SparcoursFinis(void (*appliquer)(wlSommet &))
{
  for (unsigned int i=0 ; i<this->GetNumberOfFiniteSommets() ; i++)
    (*appliquer)(this->G[i]);
}

void
wlMesh::Sparcours(wlCoin &c0, void (*appliquer)(wlSommet &))
{
  Smarque.fill(1, this->GetNumberOfSommets());
  (*appliquer)(c0.n().s());
  (*appliquer)(c0.n().n().s());
  Smarque[c0.n().s().GetId()] = Smarque[c0.n().n().s().GetId()] = 0;
  Tmarque.fill(1, this->GetNumberOfTriangles());
  this->Sparcours_rec(c0, appliquer);
  Smarque.clear();
  Tmarque.clear();
}

void
wlMesh::SparcoursFinis(wlCoin &c0, void (*appliquer)(wlSommet &))
{
  Smarque.fill(1, this->GetNumberOfSommets());
  if (!this->IsInfinite(c0.n().s()))
    (*appliquer)(c0.n().s());
  if (!this->IsInfinite(c0.n().n().s()))
    (*appliquer)(c0.n().n().s());
  Smarque[c0.n().s().GetId()] = Smarque[c0.n().n().s().GetId()] = 0;
  Tmarque.fill(1, this->GetNumberOfTriangles());
  this->Sparcours_rec(c0, appliquer, 1);
  Smarque.clear();
  Tmarque.clear();
}

void
wlMesh::Sparcours_rec(wlCoin &c, void (*appliquer)(wlSommet &), int finite)
{
  if (Smarque[c.s().GetId()]) {
    if (!finite || !this->IsInfinite(c.s()))
      (*appliquer)(c.s());
    Smarque[c.s().GetId()] = 0;
  }
  Tmarque[c.t().GetId()] = 0;
  if (Tmarque[c.r().t().GetId()])
    this->Sparcours_rec(c.r(), appliquer, finite);
  if (Tmarque[c.l().t().GetId()])
    this->Sparcours_rec(c.l(), appliquer, finite);
}

// Parcours d'un graphe en largeur d'abord
// On s'arrete quand la distance au sommet de depart est superieure a Dmax
void
wlMesh::SparcoursRegion(wlCoin &c0, void (*appliquer)(wlSommet &))
{
  // on initialise le tableau de marquage
  Smarque.fill(0, this->GetNumberOfSommets());
  // on cree une file des coins a parcourir (en fait on ne parcours qu'un seul coin d'un sommet)
  QList<wlCoin> l;
  // on y place le sommet de depart, que l'on estampille avec la distance 0 (en fait 1)
  l << c0;
  Smarque[c0.s().GetId()] = 1;
  while (!l.empty()) {
    // on extrait le premier coin de la liste
    wlCoin c = l.takeFirst();
    // on memorise la distance courante au sommet de depart
    Dcour = Smarque[c.s().GetId()]-1;
    // si la distance au sommet de depart est superieure a la distance max on arrete
    if (Dcour > Dmax)
      break;
    // on traite le sommet incident
    (*appliquer)(c.s());
    // on ajoute dans l un coin de chaque sommet adjacent non visite, que l'on estampille
    // avec la distance d+1 (en fait d+2) avec d la distance du sommet courant
    // on parcours les coins autour du sommet courant
    unsigned int id=c.GetId();
    do {
      if (!Smarque[c.n().s().GetId()]) {
	l << c.n();
	Smarque[c.n().s().GetId()] = Smarque[c.s().GetId()] + 1;
      }
      c = c.r().n();
    } while (c.GetId() != id);
  }
  Smarque.clear();
}

////////////////////////
// L'acces aux aretes //
////////////////////////

QVector<wlCoin>
wlMesh::GetAretes()
{
  this->ComputeEdges();
  return this->A;
}

QVector<wlCoin>
wlMesh::GetFiniteAretes()
{
  this->ComputeEdges();
  QVector<wlCoin> l(this->A);
  l.erase(l.begin()+this->GetNumberOfFiniteAretes(), l.end());
  return l;
}

void
wlMesh::ComputeEdges()
{
  if (this->G.isEmpty())
    return;
  if (this->A.size() == 0) {
    // afin de s'assurer que les aretes infinies seront a la fin, on les stockera
    // provisoirement dans cette structure, avant de les ajouter a la fin du traitement
    QVector<wlCoin> infinite_list;
    // on cree un tableau permettant de marquer les coins opposes afin de
    // ne pas traiter 2 fois une meme arete (une fois par chacun de ses coins)
    std::vector<int> marque(this->GetNumberOfCoins());
    fill(marque.begin(), marque.end(), 0);
    // on parcours les coins, ce qui revient a parcourir toutes les aretes,
    // et on memorise celui des 2 coins de l'arete qui a l'indice le plus faible
    for (unsigned int i=0 ; i<this->GetNumberOfCoins() ; i++) {
      if (marque[i])
	continue;
      wlCoin coin = this->C[i];
      wlCoin oppose = coin.l().n();
      if (coin.GetId() < oppose.GetId())
	if (oppose.GetId() >= this->GetNumberOfInfiniteSommets())
	  infinite_list << coin;
	else
	  this->A << coin;
      else
	if (coin.GetId() >= this->GetNumberOfInfiniteSommets())
	  infinite_list << oppose;
	else
	  this->A << oppose;
      marque[oppose.GetId()] = 1;
    }
    this->A << infinite_list;
  }
}

std::vector<std::vector<int> >
wlMesh::ComputeEdges(int finite)
{
  this->ComputeEdges();
  std::vector<std::vector<int> > edges;
  unsigned int n = finite ? this->GetNumberOfFiniteAretes() : this->GetNumberOfAretes();
  for (unsigned int i=0 ; i<n ; i++) {
    std::vector<int> l;
    l.push_back(this->A[i].s().GetId());
    l.push_back(this->A[i].n().s().GetId());
    edges.push_back(l);
  }
  return edges;
}

///////////////////////////
// L'acces aux triangles //
///////////////////////////

bool
wlMesh::IsInfinite(wlTriangle &t)
{
  return t.GetId() >= this->GetNumberOfFiniteTriangles();
}

QVector<wlTriangle>
wlMesh::GetFiniteTriangles()
{
  QVector<wlTriangle> l(this->T);
  l.erase(l.begin()+this->GetNumberOfFiniteTriangles(), l.end());
  return l;
}

std::vector<std::vector<int> >
wlMesh::GetStdTriangles()
{
  return this->ComputeTriangles(0);
}

std::vector<std::vector<int> >
wlMesh::GetStdFiniteTriangles()
{
  return this->ComputeTriangles(1);
}

std::vector<std::vector<int> >
wlMesh::ComputeTriangles(int finite)
{
  std::vector<std::vector<int> > triangles;
  // si pas de mesh on renvoie une liste vide
  if (this->G.isEmpty())
    return triangles;
  // on copie les indices des sommets des triangles dans la liste
  unsigned int n = finite ? this->GetNumberOfFiniteTriangles() : this->GetNumberOfTriangles();
  for (unsigned int i=0 ; i<n ; i++) {
    std::vector<int> l;
    l.push_back(this->C[i*3].s().GetId());
    l.push_back(this->C[i*3+1].s().GetId());
    l.push_back(this->C[i*3+2].s().GetId());
    triangles.push_back(l);
  }
  return triangles;
}

void
wlMesh::Tparcours(void (*appliquer)(wlTriangle &))
{
  for (unsigned int i=0 ; i<this->GetNumberOfTriangles() ; i++)
    (*appliquer)(this->T[i]);
}

void
wlMesh::TparcoursFinis(void (*appliquer)(wlTriangle &))
{
  for (unsigned int i=0 ; i<this->GetNumberOfFiniteTriangles() ; i++)
    (*appliquer)(this->T[i]);
}

void
wlMesh::Tparcours(wlCoin &c0, void (*appliquer)(wlTriangle &))
{
  Tmarque.fill(1, this->GetNumberOfTriangles());
  this->Tparcours_rec(c0, appliquer);
  Tmarque.clear();
}

void
wlMesh::TparcoursFinis(wlCoin &c0, void (*appliquer)(wlTriangle &))
{
  Tmarque.fill(1, this->GetNumberOfTriangles());
  this->Tparcours_rec(c0, appliquer, 1);
  Tmarque.clear();
}

void
wlMesh::Tparcours_rec(wlCoin &c, void (*appliquer)(wlTriangle &), int finite)
{
  if (!this->IsInfinite(c.t()))
    (*appliquer)(c.t());
  Tmarque[c.t().GetId()] = 0;
  if (Tmarque[c.r().t().GetId()])
    this->Tparcours_rec(c.r(), appliquer, finite);
  if (Tmarque[c.l().t().GetId()])
    this->Tparcours_rec(c.l(), appliquer, finite);
}

void
wlTriangle::Cparcours(void (*appliquer)(wlCoin &c))
{
  if (appliquer == NULL) {
    return;
  }
  for (int i=0 ; i<3 ; i++)
    (*appliquer)(this->mesh->C[this->id*3+i]);
}

///////////////////////////////
// Les deformations globales //
///////////////////////////////

void
translate(wlSommet &s)
{
  s.Deplace(vecteur);
}

void
wlMesh::Translate(QVector<float> v)
{
  vecteur = v;
  this->SparcoursFinis(translate);

  this->Modified("DisplayList");
  this->Modified("MeshSize");
}

// Fonction de deplacement d'un sommet d'une region ou la distance est euclidienne
void
Deplace(wlSommet &s)
{
  // calculer la distance au sommet de base
  float x = (float)sqrt((double)((s[0] - origine[0]) * (s[0] - origine[0]) +
                                 (s[1] - origine[1]) * (s[1] - origine[1]) +
                                 (s[2] - origine[2]) * (s[2] - origine[2])));
  if (x > dmax)
    return;
  // ramener la distance entre 0 et 1
  x /= dmax;
  // calculer le facteur de deplacement de facon a ce qu'il varie
  // de 1 a 0 du centre a la peripherie de la region a modifier
  x = (x - 1) * (x - 1) * (x + 1) * (x + 1);
  QVector<float> v;
  v << vecteur[0]*x << vecteur[1]*x << vecteur[2]*x;
  s.Deplace(v);
}

void
wlMesh::Deforme(wlSommet s, QVector<float> v, float d)
{
  if (v.size() != 3) {
    this->Error("Deforme()> le vecteur v n'est pas valide");
    return;
  }
  origine << s;
  vecteur << v;
  dmax = d;
  Sparcours(Deplace);
  fill(this->size.begin(), this->size.end(), 0);
  this->Modified("DisplayList");
  this->Modified("MeshSize");
}

// Fonction de deplacement d'un sommet d'une region ou la distance est mesuree en nombre d'aretes
void
DeplaceRegion(wlSommet &s)
{
  // calculer la distance relative
  float x = (float)Dcour / Dmax;
  // calculer le facteur de deplacement de facon a ce qu'il varie
  // de 1 a 0 du centre a la peripherie de la region a modifier
  x = (x - 1) * (x - 1) * (x + 1) * (x + 1);
  QVector<float> v;
  v << vecteur[0]*x << vecteur[1]*x << vecteur[2]*x;
  s.Deplace(v);
}

void
wlMesh::Deforme(wlSommet s, QVector<float> v, int d)
{
  if (v.size() != 3) {
    this->Error("Deforme()> le vecteur v n'est pas valide");
    return;
  }
  origine << s;
  vecteur << v;
  Dmax = d;
  // rechercher un coin incident au sommet de depart
  int i;
  for (i=0 ; this->C[i].s().GetId() != s.GetId() ; i++) {};
  SparcoursRegion(this->C[i], DeplaceRegion);
  fill(this->size.begin(), this->size.end(), 0);
  this->Modified("DisplayList");
  this->Modified("MeshSize");
}

/////////////////
// Le rendu 3D //
/////////////////

void
wlMesh::SetViewer(wlQGLViewer *v)
{
  this->viewer = v;
  if (v != NULL)
    this->makeList(1);
}

void
wlMesh::ConnectViewer()
{
  if (this->HasViewer()) {
    connect((QObject *)this->GetViewer(), SIGNAL(drawNeeded()), this, SLOT(draw()));
    if (!this->IsEmpty())
      this->ShowEntireMesh();
  }
}

void
wlMesh::ConnectViewer(wlQGLViewer *v)
{
  this->SetViewer(v);
  this->ConnectViewer();
}

void
AjouterOGLNormale(wlSommet &s)
{
  glBegin(GL_LINES);
  glVertex3f(s[0], s[1], s[2]);
  glVertex3f(s[0]+s[3], s[1]+s[4], s[2]+s[5]);
  glEnd();
}

void
AjouterOGLSommet(wlCoin &c)
{
  glVertex3fv(c.s().constData());
}

void
AjouterOGLSommetNormale(wlCoin &c)
{
  glNormal3fv(c.s().constData() + 3);
  glVertex3fv(c.s().constData());
}

void
AjouterOGLSommetTexture(wlCoin &c)
{
  glTexCoord2fv(c.s().constData() + 3);
  glVertex3fv(c.s().constData());
}

void
AjouterOGLSommetTextureNormale(wlCoin &c)
{
  glTexCoord2fv(c.s().constData() + 3);
  glNormal3fv(c.s().constData() + 5);
  glVertex3fv(c.s().constData());
}

void
AjouterOGLTriangle(wlTriangle &t)
{
  glBegin(GL_TRIANGLES);
  t.Cparcours(AjouterOGLSommet);
  glEnd();
}

void
AjouterOGLTriangleNormale(wlTriangle &t)
{
  glBegin(GL_TRIANGLES);
  t.Cparcours(AjouterOGLSommetNormale);
  glEnd();
}

void
AjouterOGLTriangleColore(wlTriangle &t)
{
  glBegin(GL_TRIANGLES);
  glColor3bv((GLbyte *)t.GetOGLColor());
  t.Cparcours(AjouterOGLSommet);
  glEnd();
}

void
AjouterOGLTriangleColoreNormale(wlTriangle &t)
{
  glBegin(GL_TRIANGLES);
  glColor3bv((GLbyte *)t.GetOGLColor());
  t.Cparcours(AjouterOGLSommetNormale);
  glEnd();
}

void
AjouterOGLTriangleTexture(wlTriangle &t)
{
  glBegin(GL_TRIANGLES);
  t.Cparcours(AjouterOGLSommetTexture);
  glEnd();
}

void
AjouterOGLTriangleTextureNormale(wlTriangle &t)
{
  glBegin(GL_TRIANGLES);
  t.Cparcours(AjouterOGLSommetTextureNormale);
  glEnd();
}

void
AjouterOGLTriangleAleatoire(wlTriangle &t)
{
  glBegin(GL_TRIANGLES);
  unsigned int i = t.GetId() % 4 + 1;
  glColor3f(i >> 2 , (i >> 1) & 0x00000001, i & 0x00000001);
  t.Cparcours(AjouterOGLSommet);
  glEnd();
}

void
AjouterOGLTriangleAleatoireNormale(wlTriangle &t)
{
  glBegin(GL_TRIANGLES);
  unsigned int i = t.GetId() % 4 + 1;
  glColor3f(i >> 2 , (i >> 1) & 0x00000001, i & 0x00000001);
  t.Cparcours(AjouterOGLSommetNormale);
  glEnd();
}

void
wlMesh::ShowEntireMesh()
{
  if (this->HasViewer()) {
    this->GetViewer()->setSceneRadius(this->GetRadius()*2);
    this->GetViewer()->setSceneCenter(qglviewer::Vec(this->GetCenter()[0], this->GetCenter()[1], this->GetCenter()[2]));
    this->GetViewer()->camera()->setRevolveAroundPoint(this->GetViewer()->sceneCenter());
    // on se place en [0, -y, 0]
    this->GetViewer()->camera()->setPosition(qglviewer::Vec(this->GetCenter()[0], this->GetCenter()[1]-this->GetRadius()*10, this->GetCenter()[2]));
    this->GetViewer()->camera()->setViewDirection(this->GetViewer()->sceneCenter() - this->GetViewer()->camera()->position());
    this->GetViewer()->camera()->setUpVector(qglviewer::Vec(0, 0, 1));
    this->GetViewer()->camera()->setFieldOfView(2 * (float)atan((double)0.11));
    /*
    this->Trace("scene center: %f %f %f", this->GetViewer()->sceneCenter()[0], this->GetViewer()->sceneCenter()[1], this->GetViewer()->sceneCenter()[2]);
    this->Trace("scene radius: %f", this->GetViewer()->sceneRadius());
    this->Trace("camera position: %f %f %f", this->GetViewer()->camera()->position()[0], this->GetViewer()->camera()->position()[1], this->GetViewer()->camera()->position()[2]);
    this->Trace("camera view direction: %f %f %f", this->GetViewer()->camera()->viewDirection()[0], this->GetViewer()->camera()->viewDirection()[1], this->GetViewer()->camera()->viewDirection()[2]);
    this->Trace("camera up vector: %f %f %f", this->GetViewer()->camera()->upVector()[0], this->GetViewer()->camera()->upVector()[1], this->GetViewer()->camera()->upVector()[2]);
    this->Trace("camera field of view: %f", this->GetViewer()->camera()->fieldOfView()/M_PI*180);
    */
  }
}

void
wlMesh::makeList(int reset_camera)
{
  if (!this->HasViewer() || !this->viewer->isValid() || this->UpToDate("DisplayList") || this->IsEmpty())
    return;
  //  this->Trace("-> makeList()");
  if (this->liste == UINT_MAX)
    if ((this->liste = glGenLists(1)) == 0) {
      this->Error("impossible de creer la display list OpenGL");
      GLenum errorCode = glGetError();
      if (errorCode != GL_NO_ERROR)
	this->Error((char *)gluErrorString(errorCode));
      this->liste = UINT_MAX;
      return;
    }
  glNewList(this->liste, GL_COMPILE);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  this->glTransformations();
  if (!this->texture.isEmpty() && this->G[0].HasTextureCoordinates()) {
    // si la mesh utilise une texture (on s'assure quand meme qu'on a bien des coordonnees de texture)
    if (this->gltexname == UINT_MAX)
      glGenTextures(1, &this->gltexname);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->gltexname);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->w, this->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->texture.constData());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    if (this->G[0].HasNormal())
      this->TparcoursFinis(AjouterOGLTriangleTextureNormale);
    else
      this->TparcoursFinis(AjouterOGLTriangleTexture);
    glDisable(GL_TEXTURE_2D);
  } else if (this->T[0].HasColor()) {
    // si des couleurs sont associees aux triangles
    if (this->G[0].HasNormal())
      this->TparcoursFinis(AjouterOGLTriangleColoreNormale);
    else
      this->TparcoursFinis(AjouterOGLTriangleColore);
  } else if (this->color.isValid()) {
    // si une couleur unique est associee a la mesh
    glColor3f(this->color.redF(), this->color.greenF(), this->color.blueF());
    if (this->G[0].HasNormal())
      this->TparcoursFinis(AjouterOGLTriangleNormale);
    else
      this->TparcoursFinis(AjouterOGLTriangle);
  } else {
    // sinon on attribue des couleurs aleatoires aux triangles
    if (this->G[0].HasNormal())
      this->TparcoursFinis(AjouterOGLTriangleAleatoireNormale);
    else
      this->TparcoursFinis(AjouterOGLTriangleAleatoire);
  }
  glPopMatrix();
  glEndList();
  if (reset_camera)
    this->ShowEntireMesh();
  this->Updated("DisplayList");
  //  this->Trace("<- makeList()");
}

void
wlMesh::makeList(wlCoin c0, int reset_camera)
{
  if (!this->HasViewer() || !this->viewer->isValid() || this->UpToDate("DisplayList") || this->IsEmpty())
    return;
  //  this->Trace("-> makeList(%u)", c0.GetId());
  if (this->liste == UINT_MAX)
    if ((this->liste = glGenLists(1)) == 0) {
      this->Error("impossible de creer la display list OpenGL");
      GLenum errorCode = glGetError();
      if (errorCode != GL_NO_ERROR)
	this->Error((char *)gluErrorString(errorCode));
      this->liste = UINT_MAX;
      return;
    }
  glNewList(this->liste, GL_COMPILE);
  if (!this->texture.isEmpty() && this->G[0].HasTextureCoordinates()) {
    // si la mesh utilise une texture (on s'assure quand meme qu'on a bien des coordonnees de texture)
    if (this->gltexname == UINT_MAX)
      glGenTextures(1, &this->gltexname);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->gltexname);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->w, this->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->texture.constData());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    if (this->G[0].HasNormal())
      this->TparcoursFinis(c0, AjouterOGLTriangleTextureNormale);
    else
      this->TparcoursFinis(c0, AjouterOGLTriangleTexture);
    glDisable(GL_TEXTURE_2D);
  } else if (this->T[0].HasColor()) {
    // si des couleurs sont associees aux triangles
    if (this->G[0].HasNormal())
      this->TparcoursFinis(c0, AjouterOGLTriangleColoreNormale);
    else
      this->TparcoursFinis(c0, AjouterOGLTriangleColore);
  } else if (this->color.isValid()) {
    // si une couleur unique est associee a la mesh
    glColor3f(this->color.redF(), this->color.greenF(), this->color.blueF());
    if (this->G[0].HasNormal())
      this->TparcoursFinis(c0, AjouterOGLTriangleNormale);
    else
      this->TparcoursFinis(c0, AjouterOGLTriangle);
  } else {
    // sinon on attribue des couleurs aleatoires aux triangles
    if (this->G[0].HasNormal())
      this->TparcoursFinis(c0, AjouterOGLTriangleAleatoireNormale);
    else
      this->TparcoursFinis(c0, AjouterOGLTriangleAleatoire);
  }
  glEndList();
  if (reset_camera)
    this->ShowEntireMesh();
  this->Updated("DisplayList");
  //  this->Trace("<- makeList()");
}

GLuint
wlMesh::GetList(int reset_camera)
{
  //this->Trace("GetList(%d)", reset_camera);
  if (!this->UpToDate("DisplayList"))
    this->makeList(reset_camera);
  return this->liste;
}

void
wlMesh::SetShaderList(QList<QGLShader *> shaders)
{
  this->shaders = shaders;
  this->pshader->removeAllShaders();
  for (int i=0 ; i<shaders.size() ; i++)
    this->pshader->addShader(shaders[i]);
  this->pshader->link();
  this->pshader->bind();
}

void
wlMesh::AddShader(QGLShader *shader)
{
  this->shaders << shader;
  this->pshader->addShader(shader);
  this->pshader->link();
  this->pshader->bind();
}

void
wlMesh::AddVertexShaderFromFile(QString filename)
{
  QGLShader *shader = new QGLShader(QGLShader::Vertex);
  if (!shader->compileSourceFile(filename))
    this->Error("::AddVertexShaderFromFile: error while compiling vertex file \"%s\"", filename.toAscii().constData());
  else
    this->AddShader(shader);
}

void
wlMesh::AddFragmentShaderFromFile(QString filename)
{
  QGLShader *shader = new QGLShader(QGLShader::Fragment);
  if (!shader->compileSourceFile(filename))
    this->Error("::AddFragmentShaderFromFile: error while compiling fragment file \"%s\"", filename.toAscii().constData());
  else
    this->AddShader(shader);
}

void
wlMesh::draw()
{
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCallList(this->GetList());
  glEnable(GL_LIGHT0);
  float light_position[] = { 0, -this->GetRadius()*10, this->GetRadius()*10, 1.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glFlush();
}
