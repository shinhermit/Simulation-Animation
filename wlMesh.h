///////////////////////////////////////////////////////////////////////////////
//  File         :    wlMesh.h                                               //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Classe implementant une mesh avec gestion des          //
//                    relations topologiques, insertions/suppressions, etc   //
//  Related file :    wlMesh.cxx                                             //
//                    Wilfrid Lefer - UPPA - 2007                            //
///////////////////////////////////////////////////////////////////////////////


#ifndef _WL_MESH_H
#define _WL_MESH_H

#include <wlQGLViewer.h>
#include <wlCore.h>
#include <QObject>
#include <QVector>
#include <QGLShader>
#include <QGLShaderProgram>
#include <math.h>
#include <glu.h>

/** \todo Dans wlMesh::GetEdges() on s'arrete si la taille de la liste edges est egale au nombre
    de coins / 2 mais il faut reflechir au cas du modele de terrain (surface ouverte)
*/

// on declare les classes ici car le compilateur Linux, contrairement au compilateur Mac,
// ne sait pas deduire de la declaration "friend class wlCoin" que wlCoin est une classe !
class wlCoin;
class wlSommet;
class wlTriangle;

/** \brief wlMesh est une classe implementant une mesh avec gestion des relations
    topologiques, insertions/suppressions, etc.

    Attention lors de l'enregistrement des elements ! Il faut absolument serialiser pour que
    l'affectation des identifiants soit correcte, eviter en particulier les instructions du genre :
    \code
      this->C << wlCoin(this) << wlCoin(this) << wlCoin(this);
    \endcode
    mais au contraire ecrire :
    \code
      this->C << wlCoin(this);
      this->C << wlCoin(this);
      this->C << wlCoin(this);
    \endcode

    Definir des operateurs topologiques consistants sur une mesh implique generalement l'absence
    de bords et cette classe ne fonctionnera que pour des meshes sans bord !
    Cependant certaines mesh, tels que les modeles de terrain, ont typiquement des bords.
    La bonne solution afin de gerer correctement cette situation consiste a ce que l'utilisateur
    ajoute un sommet "bidon", typiquement suppose a l'infini, qui est connecte a l'enveloppe
    convexe de la mesh ouverte, ce qui permet de la fermer. Ce sommet infini est du coup adjacent
    a des aretes et des triangles infinis. Les methodes IsInfinite() permettent de tester la
    nature des sommets, aretes et coins afin de distinguer ceux qui sont infinis des autres.
    Cette distinction n'a aucun impact sur le fonctionnement de la presente classe mais il permet
    de coder cette propriete, qui pourra etre utile a un algorithme utilisant cette classe, qui
    traiterait par exemple differemment les bords de la mesh de son interieur.
    Attention toutefois : lors de l'initialisation d'une mesh, les sommets infinis doivent etre
    a la fin de la liste des sommets et les triangles infinis a la fin de la liste des triangles.

    Il est important de gerer correctement la relation entre la mesh et son viewer selon l'utilisation
    que l'on veut en faire. Il est typiquement 3 cas de figure :
    <ul>
      <li>la mesh n'est pas visualisee mais seulement manipulee en arriere plan : il n'est alors pas
          necessaire - et meme probablement pas souhaitable - de la connecter a un viewer et on
          l'instanciera alors avec la valeur NULL pour le parametre <em>v</em>.</li>
      <li>la mesh est le seul objet 3D visualise dans le viewer : on devra alors enregistrer ce viewer,
          soit en le passant en parametre lors de l'instanciation, soit en appelant la methode
          <em>SetViewer()</em>, puis le connecter en appelant la methode <em>ConnectViewer()</em>
          afin qu'il rafraichisse l'image en appelant la methode <em>draw()</em> de la mesh.
          Les 2 appels peuvent etre regroupes en passant le viewer en parametre de la methode <em>ConnectViewer()</em>.</li>
      <li>plusieurs objets doivent etre visualises dans un viewer unique et cette mesh est un de ces objets :
          on devra alors enregistrer ce viewer, en le passant en parametre lors de l'instanciation ou en
          appelant la methode <em>SetViewer()</em>, mais sans le connecter. Il sera alors necessaire de
          redefinir une methode <em>draw()</em> au sein de la classe qui se chargera de gerer l'affichage
          de tous les objets. Toutefois remarquons que dans ce cas l'objet <em>wlMesh</em> se chargera
          de la creation de la display list OpenGL, l'objet afficheur n'ayant qu'a la recuperer en
          invoquant la methode <em>GetList()</em>.</li>
    </ul>
*/
class wlMesh : public QObject, public wlCore
{
  Q_OBJECT

  friend class wlSommet;
  friend class wlTriangle;
  friend class wlCoin;

public:
  /// Initialise une mesh a partir du contenu d'un fichier ou cree une mesh par defaut (un tetrahedre) si <em>filename</em> vaut NULL.
  /// Une texture peut egalement etre associee a la mesh.
  /// Eventuellement connecte cette mesh a un viewer de type wlQGLViewer.
  wlMesh(int debug=0, wlQGLViewer *v=NULL, QString fname=QString(), QString tname=QString());
  /// Cree une mesh qui soit une copie conforme d'une autre mesh.
  /// Attention : la nouvelle mesh n'est pas connectee au viewer, il faudra pour cela appeler explicitement ConnectViewer().
  wlMesh(wlMesh *mesh);
  virtual ~wlMesh() {}
  virtual char *GetClassName() {return "wlMesh";}

  /// \brief Associe un viewer de type wlQGLViewer a cette mesh.
  /// Si cette mesh est le seul objet que doit gerer ce viewer, il est necessaire d'appeler ensuite la methode <em>ConnectViewer()</em>
  /// afin que le viewer fasse appel a la methode <em>draw()</em> de cet objet afin qu'il se redessine. La methode
  ///  <em>ConnectViewer(wlQGLViewer *)</em> equivaut a <em>SetViewer(wlQGLViewer *)</em> suivi de <em>ConnectViewer()</em>.
  void SetViewer(wlQGLViewer *viewer);
  /// Renvoie vrai si une viewer a ete connecte a cette mesh.
  inline bool HasViewer() {return this->GetViewer()!=NULL;}
  /// Renvoie le viewer de type wlQGLViewer.
  inline wlQGLViewer *GetViewer() {return this->viewer;}
  /// \brief Connecte la mesh avec un viewer enregistre au prealable par appel a <em>SetViewer(wlQGLViewer *)</em>.
  /// Cette methode doit etre appelee lorsque la presente mesh est le seul objet a visualiser dans ce viewer.
  /// Ainsi le viewer mettra a jour son affichage par appel a la methode <em>draw()</em> de cet objet.
  void ConnectViewer();
  /// \brief Connecte la mesh avec un viewer de type wlQGLViewer.
  /// Cette methode est equivalente a appeler <em>SetViewer(wlQGLViewer *)</em> suivi de <em>ConnectViewer()</em>.
  void ConnectViewer(wlQGLViewer *v);
  /// \brief Renvoie la display list OpenGL pour la presente mesh.
  /// Si la mesh doit etre reconstruite, la camera est eventuellement reinitialisee de facon a voir l'ensemble de la mesh.
  GLuint GetList(int reset_camera=0);
  /// \brief Renvoie la liste de shaders utilises pour le rendu de la mesh.
  QList<QGLShader *> GetShaderList() {return this->shaders;}
  /// \brief Associe une liste de shaders afin de gerer le rendu de la mesh.
  void SetShaderList(QList<QGLShader *> shaders);
  /// \brief Ajoute un shader a la liste des shaders.
  void AddShader(QGLShader *shader);
  /// \brief Lit un vertex shader dans un fichier et l'ajoute a la liste des shaders.
  void AddVertexShaderFromFile(QString filename);
  /// \brief Lit un fragment shader dans un fichier et l'ajoute a la liste des shaders.
  void AddFragmentShaderFromFile(QString filename);

  /// Renvoie vrai si cette mesh est vide.
  inline bool IsEmpty() {return this->G.size()==0;}
  /// Renvoie la taille de la mesh au format (<xdim>, <ydim>, <zdim>).
  std::vector<float> GetSize();
  /// Renvoie les limites de la mesh au format (<xmin>, <xmax>, <ymin>, <ymax>, <zmin>, <zmax>).
  std::vector<float> GetExtends();
  /// Renvoie la centre de la mesh au format (<x>, <y>, <z>).
  std::vector<float> GetCenter();
  /// Renvoie le rayon (largeur maximale) de la mesh.
  /// Cette methode calcule en fait la demi-diagonale du cube englobant, aligne sur les axes.
  /// Cette methode peut etre surchargee afin d'affiner le calcul.
  virtual float GetRadius();

  /// Renvoie vrai s'il existe un sommet infini dans la mesh (et donc des aretes et triangles infinis).
  inline bool HasInfiniteSommet() {return this->nb_infinite_sommets;}
  /// Renvoie le nombre de coins de la mesh.
  inline unsigned int GetNumberOfCoins() {return this->C.size();}
  /// Renvoie le nombre de coins finis de la mesh.
  inline unsigned int GetNumberOfFiniteCoins() {return this->GetNumberOfCoins() - this->GetNumberOfInfiniteCoins();}
  /// Renvoie le nombre de coins infinis de la mesh.
  inline unsigned int GetNumberOfInfiniteCoins() {return this->nb_infinite_coins;}
  /// Renvoie le nombre de sommets de la mesh.
  inline unsigned int GetNumberOfSommets() {return this->G.size();}
  /// Renvoie le nombre de sommets finis de la mesh.
  inline unsigned int GetNumberOfFiniteSommets() {return this->GetNumberOfSommets() - this->GetNumberOfInfiniteSommets();}
  /// Renvoie le nombre de sommets infinis de la mesh.
  inline unsigned int GetNumberOfInfiniteSommets() {return this->nb_infinite_sommets;}
  /// Renvoie le nombre d'aretes de la mesh.
  inline unsigned int GetNumberOfAretes() {return this->C.size() / 2;}
  /// Renvoie le nombre d'aretes finies de la mesh.
  inline unsigned int GetNumberOfFiniteAretes() {return this->GetNumberOfFiniteCoins() / 2;}
  /// Renvoie le nombre d'aretes infinies de la mesh.
  inline unsigned int GetNumberOfInfiniteAretes() {return this->GetNumberOfInfiniteCoins() / 2;}
  /// Renvoie le nombre de triangles de la mesh.
  inline unsigned int GetNumberOfTriangles() {return this->T.size();}
  /// Renvoie le nombre de triangles finis de la mesh.
  inline unsigned int GetNumberOfFiniteTriangles() {return this->GetNumberOfTriangles() - this->GetNumberOfInfiniteTriangles();}
  /// Renvoie le nombre de triangles infinis de la mesh.
  inline unsigned int GetNumberOfInfiniteTriangles() {return this->nb_infinite_triangles;}

  /// Renvoie le coin d'identifiant <em>i</em>
  inline wlCoin &GetCoin(int i) {return this->C[i];}
  /// Renvoie le sommet d'identifiant <em>i</em>
  inline wlSommet &GetSommet(int i) {return this->G[i];}
  /// Renvoie le triangle d'identifiant <em>i</em>
  inline wlTriangle &GetTriangle(int i) {return this->T[i];}

  /// Renvoie vrai si le coin <em>c</em> appartient a un triangle infini.
  bool IsInfinite(wlCoin &c);
  /// Renvoie vrai si le sommet <em>s</em> est infini.
  bool IsInfinite(wlSommet &s);
  /// Renvoie vrai si le triangle <em>t</em> est infini.
  bool IsInfinite(wlTriangle &t);

  /// Parcours la liste des coins (i.e. des aretes) et applique une fonction a chacun d'eux.
  void Cparcours(void (*appliquer)(wlCoin &));
  /// Parcours la liste des coins (i.e. des aretes) finis et applique une fonction a chacun d'eux.
  void CparcoursFinis(void (*appliquer)(wlCoin &));
  /// Parcours la liste des coins (i.e. des aretes) a partir d'un coin donne et applique une fonction a chacun d'eux.
  void Cparcours(wlCoin &c0, void (*appliquer)(wlCoin &));
  /// Parcours la liste des coins (i.e. des aretes) finis a partir d'un coin donne et applique une fonction a chacun d'eux.
  void CparcoursFinis(wlCoin &c0, void (*appliquer)(wlCoin &));
  /// Parcours la liste des sommets et applique une fonction a chacun d'eux.
  void Sparcours(void (*appliquer)(wlSommet &));
  /// Parcours la liste des sommets finis et applique une fonction a chacun d'eux.
  void SparcoursFinis(void (*appliquer)(wlSommet &));
  /// Parcours la liste des sommets a partir d'un coin donne et applique une fonction a chacun d'eux.
  void Sparcours(wlCoin &c0, void (*appliquer)(wlSommet &));
  /// Parcours la liste des sommets finis a partir d'un coin donne et applique une fonction a chacun d'eux.
  void SparcoursFinis(wlCoin &c0, void (*appliquer)(wlSommet &));
  /// Parcours tous les sommets situes a une distance maximale <em>d</em> d'un sommet donne par
  /// un de ses coins <em>c0</em> et applique une fonction a chacun d'eux.
  void SparcoursRegion(wlCoin &c0, void (*appliquer)(wlSommet &));
  /// Parcours la liste des triangles et applique une fonction a chacun d'eux.
  void Tparcours(void (*appliquer)(wlTriangle &));
  /// Parcours la liste des triangles finis et applique une fonction a chacun d'eux.
  void TparcoursFinis(void (*appliquer)(wlTriangle &));
  /// Parcours la liste des triangles a partir d'un coin donne et applique une fonction a chacun d'eux.
  void Tparcours(wlCoin &c0, void (*appliquer)(wlTriangle &));
  /// Parcours la liste des triangles finis a partir d'un coin donne et applique une fonction a chacun d'eux.
  void TparcoursFinis(wlCoin &c0, void (*appliquer)(wlTriangle &));

  /// Associe une texture a la mesh.
  void SetTexture(QImage image);
  /// Calcule les normales aux sommets de la mesh.
  void ComputeNormals();

signals:
  void numberOfTrianglesHasChanged(int);

public slots:
  /// Renvoie une copie de la table des coins.
  inline QVector<wlCoin> GetCoins() {return QVector<wlCoin>(this->C);};
  /// Renvoie une copie de la table des coins finis.
  QVector<wlCoin> GetFiniteCoins();
  /// Renvoie une copie de la table des sommets.
  QVector<wlSommet> GetSommets() {return QVector<wlSommet>(this->G);};
  /// Renvoie une copie de la table des sommets finis.
  QVector<wlSommet> GetFiniteSommets();
  /// Renvoie une copie de la table des sommets au format STL.
  std::vector<std::vector<float> > GetStdSommets();
  /// Renvoie une copie de la table des sommets finis au format STL.
  std::vector<std::vector<float> > GetStdFiniteSommets();
  /// Renvoie la liste des aretes sous forme de coins (dualite coin-arete).
  /// Le coin correspond au sommet de plus petit indice.
  QVector<wlCoin> GetAretes();
  /// Renvoie la liste des aretes finies sous forme de coins (dualite coin-arete).
  /// Le coin correspond au sommet de plus petit indice.
  QVector<wlCoin> GetFiniteAretes();
  /// Renvoie la liste des aretes sous forme de couple d'indices des sommets.
  /// Le sommet de plus petit indice est toujours donne en premier.
  std::vector<std::vector<int> > GetStdAretes() {return this->ComputeEdges(0);};
  /// Renvoie la liste des aretes finies sous forme de couple d'indices des sommets.
  /// Le sommet de plus petit indice est toujours donne en premier.
  std::vector<std::vector<int> > GetStdFiniteAretes() {return this->ComputeEdges(1);};
  /// Renvoie une copie de la table des triangles.
  QVector<wlTriangle> GetTriangles() {return QVector<wlTriangle>(this->T);};
  /// Renvoie une copie de la table des triangles finis.
  QVector<wlTriangle> GetFiniteTriangles();
  /// Renvoie une copie de la table des triangles au format STL.
  std::vector<std::vector<int> > GetStdTriangles();
  /// Renvoie une copie de la table des triangles finis au format STL.
  std::vector<std::vector<int> > GetStdFiniteTriangles();

  /// Deplace la mesh par translation de vecteur v.
  void Translate(QVector<float> v);
  /// Deforme la mesh par translation du sommet s de vecteur v.
  /// La region de la mesh qui est deformee est definie par la sphere de centre s et de rayon d.
  void Deforme(wlSommet s, QVector<float> v, float d);
  /// Deforme la mesh par translation du sommet s de vecteur v.
  /// La region de la mesh qui est deformee est definie par la portion de graphe de centre s et de rayon d aretes.
  void Deforme(wlSommet s, QVector<float> v, int d);

  /// \brief Charge une nouvelle mesh depuis un fichier.
  /// Attention : seul le format OFF est pour l'instant en partie supporte :
  /// <ul><li>format OFF basique, tel que decrit <a href="http://people.scs.fsu.edu/~burkardt/data/off/off.html">la</a></li>
  ///     <li>uniquement des faces triangulaires : rencontrer une face non triangulaire genere une erreur</li>
  ///     <li>pas d'attributs de couleur : les attributs de couleurs sont ignores</li>
  /// </ul>
  /// Si <em>filename</em> est vide, le nom du fichier doit avoir ete enregistre par appel prealable a <em>SeFilename()</em>.
  virtual void LoadMesh(QString filename=QString());
  /// Renvoie le nom du fichier contenant la mesh courante si elle a ete chargee depuis un fichier.
  inline QString GetFilename() {return this->fname;};
  /// Initialise la mesh avec une liste de sommets et une liste de triangles.
  /// Ne pas oublier de preciser le nombre d'aretes et de triangles infinis si l'objet n'est pas
  /// homeomorphe a la sphere topologique (voir explication au debut de cette page) !
  virtual void InitMesh(std::vector<std::vector<float> > vertices, std::vector<std::vector<int> > triangles,
			unsigned int HasInfiniteVertex=0, unsigned int NbInfiniteTriangles=0);
  /// Affecte une couleur a la mesh.
  void SetColor(QColor color) {this->color = color;}
  /// Charge une texure a partir d'un fichier image.
  void LoadTexture(QString filename);
  /// Sauvegarde la mesh dans un fichier au format OFF.
  void SaveMeshAsOFF(QString filename);

  virtual void draw();
  /// \brief Construit la display list pour la mesh et eventuellement reinitialise la camera de facon a voir l'ensemble de la mesh.
  /// Cette methode est a utiliser lorsque la mesh n'est qu'un des objets a visualiser.
  virtual void makeList(int reset_camera=0);
  /// \brief Construit la display list pour la mesh en la parcourant a parir d'un de ses coins et eventuellement reinitialise la camera de facon a voir l'ensemble de la mesh.
  /// Cette methode est a utiliser lorsque la mesh n'est qu'un des objets a visualiser.
  virtual void makeList(wlCoin c0, int reset_camera=0);
  /// Positionne la camera de facon a voir l'ensemble de la mesh.
  virtual void ShowEntireMesh();

  /// Imprime le contenu d'une mesh : coordonnees des sommets, aretes et faces.
  void PrintSelf();
  /// Imprime le contenu detaille d'une mesh : contenu des differentes tables.
  void PrintContent();
  // vide toutes les structures internes
  void Clear();

protected:
  // la liste des sommets : la table G
  QVector<wlSommet> G;
  // la liste des triangles : la table T
  QVector<wlTriangle> T;
  // la liste des coins
  QVector<wlCoin> C;
  // la liste des aretes : elle n'est construite qu'a la demande
  QVector<wlCoin> A;
  // la table S
  QVector<unsigned int> S;
  // la table O
  QVector<unsigned int> O;
  // le nombre de sommets infinis (en fait 0 ou 1)
  unsigned int nb_infinite_sommets;
  // le nombre de triangles infinis
  unsigned int nb_infinite_triangles;
  // le nombre de coins infinis
  unsigned int nb_infinite_coins;
  // la taille de la mesh
  std::vector<float> size;
  // les limites de la mesh
  std::vector<float> extends;
  // le centre de la mesh
  std::vector<float> center;
  // le rayon de la mesh
  float radius;
  // la texture associee a la mesh
  unsigned int w, h;
  QVector<unsigned int> texture;
  GLuint gltexname;
  // la couleur associee a la mesh
  QColor color;
  // le nom du fichier contenant la mesh
  QString fname;
  // le nom du fichier contenant la texture a plaquer sur la mesh
  QString tname;
  // le viewer
  wlQGLViewer *viewer;
  // le numero de la liste d'affichage pour la mesh
  GLuint liste;
  // la liste des shaders utilises pour le rendu
  QList<QGLShader *> shaders;
  // le shader program pour utiliser tout ca
  QGLShaderProgram *pshader;

  // Lit un fichier au format OFF
  void LoadOFFFile(QString filename);
  // Construit la table O.
  void BuildOTable();
  // la fonction recursive pour le parcours des coins a partir d'un coin
  void Cparcours_rec(wlCoin &c, void (*appliquer)(wlCoin &), int finite=0);
  // la fonction recursive pour le parcours des sommets a partir d'un coin (voir cours)
  void Sparcours_rec(wlCoin &c, void (*appliquer)(wlSommet &), int finite=0);
  // la fonction recursive pour le parcours des triangles a partir d'un coin (voir cours)
  void Tparcours_rec(wlCoin &c, void (*appliquer)(wlTriangle &), int finite=0);
  // construit la liste des aretes
  void ComputeEdges();
  // construit la liste des aretes au format STL (couples de sommets), incluant ou non les aretes infinies
  std::vector<std::vector<int> > ComputeEdges(int finite);
  // construit la liste des triangles au format STL, incluant ou non les triangles infinis
  std::vector<std::vector<int> > ComputeTriangles(int finite);
  // cette methode peut etre surchargee afin d'appliquer des transformations geometriques OpenGL a l'objet
  virtual void glTransformations() {};
};

/** \brief wlSommet represente un point en 3D.
*/
class wlSommet : public QVector<float>, public wlCore
{
public:
  /// Provisoire : constructeur bidon afin que ca passe a la compile (attente d'une reponse au probleme pose sur QtCentre).
  wlSommet() {};
  /// Initialise un sommet a partir d'un tableau contenant ses 3 coordonnees.
  wlSommet(wlMesh *mesh, float xyz[3]) {
    this->mesh = mesh;
    this->id = mesh->G.size();
    *this << xyz[0] << xyz[1] << xyz[2];
  };
  /// Initialise un sommet a partir de ses 3 coordonnees.
  wlSommet(wlMesh *mesh, float x, float y, float z) {
    this->mesh = mesh;
    this->id = mesh->G.size();
    *this << x << y << z;
  };
  /// Initialise un sommet a partir de ses 3 coordonnees et ses coordonnees de texture.
  /// Les coordonnees de textures peuvent aussi etre utilisees afin de stocker une couleur.
  wlSommet(wlMesh *mesh, float x, float y, float z, float u, float v) {
    this->mesh = mesh;
    this->id = mesh->G.size();
    *this << x << y << z << u << v;
  };
  /// Initialise un sommet a partir de ses 3 coordonnees et sa normale.
  wlSommet(wlMesh *mesh, float x, float y, float z, float nx, float ny, float nz) {
    this->mesh = mesh;
    this->id = mesh->G.size();
    *this << x << y << z << nx << ny << nz;
  };
  /// Initialise un sommet a partir de ses 3 coordonnees, ses coordonnees de texture et sa normale.
  /// Les coordonnees de textures peuvent aussi etre utilisees afin de stocker une couleur.
  wlSommet(wlMesh *mesh, float x, float y, float z, float u, float v, float nx, float ny, float nz) {
    this->mesh = mesh;
    this->id = mesh->G.size();
    *this << x << y << z << nx << ny << nz << u << v;
  };
  /// Initialise un sommet a partir d'un vecteur STL contenant ses 3 coordonnees, eventuellement
  /// ses coordonnees de texture et sa normale (2, 3 ou 5 coordonnees supplementaires).
  /// Les coordonnees de textures peuvent aussi etre utilisees afin de stocker une couleur.
  wlSommet(wlMesh *mesh, std::vector<float> xyz) {
    this->mesh = mesh;
    this->id = mesh->G.size();
    (*this) << QVector<float>::fromStdVector(xyz);
  };
  /// Initialise un sommet comme etant le point mitoyen entre 2 autres sommets.
  wlSommet(wlMesh *mesh, wlSommet &s1, wlSommet &s2) {
    this->mesh = mesh;
    this->id = mesh->G.size();
    for (int i=0 ; i<s1.size() && i<s2.size() ; i++)
      *this << (s1[i]+s2[i])/2;
  };
  virtual ~wlSommet() {};
  virtual char *GetClassName() {return "wlSommet";};

  /// Imprime les coordonnees du sommet, ses coordonnees de texture et sa normale.
  void PrintSelf() {
    for (int i=0 ; i<this->size() ; i++)
      this->Print(" %.2f\r", (*this)[i]);
    this->Print("\n");
  };
  /// Renvoie l'identifiant du sommet.
  unsigned int GetId() {return this->id;};
  /// Renvoie la mesh a laquelle appartient ce sommet.
  inline wlMesh *GetMesh() {return this->mesh;};

  /// Renvoie vrai si le sommet comporte une normale.
  inline bool HasNormal() {return this->size() > 5;};
  /// Renvoie vrai si le sommet comporte des coordonnees de texture.
  inline bool HasTextureCoordinates() {return this->size() == 5 || this->size() == 8;};

  /// Renvoie la normale au sommet, qui peut etre un vecteur vide s'il n'y a pas de normale.
  QVector<float> GetNormal() {
    if (this->HasNormal())
      return this->mid(this->HasTextureCoordinates() ? 5 : 3, 3);
    else
      return QVector<float>();
  };
  /// Renvoie la normale au sommet, qui peut etre un vecteur vide s'il n'y a pas de normale.
  inline std::vector<float> GetStdNormal() {return this->GetNormal().toStdVector();};

  /// Calcule la distance a un autre sommet.
  float Distance(wlSommet s) {
    return (float)sqrt((double)(((*this)[0] - s[0]) * ((*this)[0] - s[0]) +
                                ((*this)[1] - s[1]) * ((*this)[1] - s[1]) +
                                ((*this)[2] - s[2]) * ((*this)[2] - s[2])));
  };
  /// Deplace le sommet d'un vecteur v.
  void Deplace(QVector<float> v) {
    (*this)[0] += v[0];
    (*this)[1] += v[1];
    (*this)[2] += v[2];
    this->mesh->Modified("DisplayList");
    this->mesh->Modified("MeshSize");
  };
  // Ajoute un vecteur a la normale au sommet
  // Cette fonction interne est utile pour le calcul des normales, elle permet de cumuler les normales aux faces incidentes
  void AddNormal(QVector<float> N) {
    if (!this->HasNormal()) {
      QVector<float> n(3, 0);
      *this += n;
    }
    unsigned int i = this->HasTextureCoordinates() ? 5 : 3;
    (*this)[i] += N[0]; (*this)[i+1] += N[1]; (*this)[i+2] += N[2];
  };
  // Normalise la normale si elle existe
  void Normalize() {
    if (this->HasNormal()) {
      unsigned int i = this->HasTextureCoordinates() ? 5 : 3;
      float x = (float)sqrt((double)((*this)[i] * (*this)[i] + (*this)[i+1] * (*this)[i+1] + (*this)[i+2] * (*this)[i+2]));
      (*this)[i] /= x; (*this)[i+1] /= x; (*this)[i+2] /= x;
      this->mesh->Modified("DisplayList");
    }
  };

private:
  // la mesh
  wlMesh *mesh;
  // le numero du sommet, ie son indice dans la table des sommets
  unsigned int id;
};

/** \brief wlTriangle represente une face en 3D.

    La liste des coins d'un triangle n'est pas stockee de maniere explicite car
    elle peut etre obtenue a partir de la table V : (c0,c1,c2)=(V[id*3],V[id*3+1],V[id*3+2]).
*/
class wlTriangle : public wlCore
{
public:
  /// Provisoire : constructeur bidon afin que ca passe a la compile (attente d'une reponse au probleme pose sur QtCentre).
  wlTriangle() {};
  /// Initialise un triangle.
  wlTriangle(wlMesh *mesh) {
    this->mesh = mesh;
    this->id = mesh->T.size();
  };
  /// Initialise un triangle avec sa couleur (format 0xRRGGBBAA).
  wlTriangle(wlMesh *mesh, unsigned int rgba) {
    this->mesh = mesh;
    this->id = mesh->T.size();
    this->color.push_back(rgba);
  };
  virtual ~wlTriangle() {};
  virtual char *GetClassName() {return "wlTriangle";};

  /// Imprime les references (entiers) des sommets incidents a la face, dans l'ordre clockwise et eventuellement sa couleur.
  void PrintSelf() {
    if (this->HasColor())
      this->Print("  %d %d %d (%u, %u, %u, %u)", this->mesh->S[this->id*3], this->mesh->S[this->id*3+1], this->mesh->S[this->id*3+2],
		  (this->GetColor() & 0xff000000) >> 24, (this->GetColor() & 0x00ff0000) >> 16,
		  (this->GetColor() & 0x0000ff00) >> 8, this->GetColor() & 0x000000ff);
    else
      this->Print("  %d %d %d", this->mesh->S[this->id*3], this->mesh->S[this->id*3+1], this->mesh->S[this->id*3+2]);
  };

  /// Renvoie vrai si le triangle a une couleur associee.
  inline bool HasColor() {return this->color.size();};
  /// Renvoie la couleur du triangle.
  inline unsigned int GetColor() {return this->color[0];};
  /// Renvoie un iterateur sur l'indice du premier sommet du triangle dans la table des indices.
  /// Cette methode est a priviligier si on veut uniquement acceder aux sommets car elle ne fait
  /// pas de copie, contrairement a <em>QVector<int> GetIndices()</em>.
  inline QVector<unsigned int>::const_iterator GetIndexIterator() {return this->mesh->S.begin() + this->GetId()*3;};
  /// Renvoie une copie des indices des sommets du triangle dans la table des sommets. Cette methode fait une copie effective des donnees !
  inline QVector<int> GetIndices() {
    QVector<int> idx(3);
    idx[0] = this->mesh->S[this->GetId()*3];
    idx[1] = this->mesh->S[this->GetId()*3+1];
    idx[2] = this->mesh->S[this->GetId()*3+2];
    return idx;
  };
  /// Renvoie un pointeur sur la couleur du triangle. Cette methode est fournie afin de faciliter les appels a la methode glColor3bv() de OpenGL.
  inline unsigned int *GetOGLColor() {return &this->color[0];};

  /// Parcours la liste des coins incidents et eventuellement applique une fonction a chacun d'eux.
  void Cparcours(void (*appliquer)(wlCoin &)=NULL);
  /// Renvoie l'identifiant du triangle.
  unsigned int GetId() {return this->id;};
  /// Renvoie la mesh a laquelle appartient ce triangle.
  inline wlMesh *GetMesh() {return this->mesh;};

private:
  // la mesh
  wlMesh *mesh;
  // le numero du triangle, ie son indice dans la table des triangles
  unsigned int id;
  // la couleur du triangle
  std::vector<unsigned int> color;
};

/** \brief wlCoin represente un coin d'un triangle.
*/
class wlCoin : public wlCore
{
public:
  /// Provisoire : constructeur bidon afin que ca passe a la compile (attente d'une reponse au probleme pose sur QtCentre).
  wlCoin() {};
  /// Initialise un coin d'une mesh.
  wlCoin(wlMesh *mesh) {
    this->mesh = mesh;
    this->id = mesh->C.size();
  };
  virtual ~wlCoin() {};
  virtual char *GetClassName() {return "wlCoin";};

  /// Imprime le numero du coin.
  void PrintSelf() {this->Print("  %d", this->id);};

  /// Operateur .s : renvoie le sommet incident au coin.
  wlSommet &s() {return this->mesh->G[this->mesh->S[this->id]];};
  /// Operateur .t : renvoie le triangle incident au coin.
  wlTriangle &t() {return this->mesh->T[this->id / 3];};
  /// Operateur .n : renvoie le coin suivant dans le triangle courant.
  wlCoin &n() {return this->mesh->C[this->id / 3 * 3 + (this->id + 1) % 3];};
  /// Operateur .o : renvoie le coin symetrique (dans triangle adjacent) par rapport a l'arete opposee.
  wlCoin &o() {return this->mesh->C[this->mesh->O[this->id]];};
  /// Operateur .r : renvoie le coin oppose a l'arete de droite (en regardant le coin vers le haut).
  wlCoin &r() {return this->mesh->C[this->n().o().GetId()];};
  /// Operateur .l : renvoie le coin oppose a l'arete de gauche (en regardant le coin vers le haut).
  wlCoin &l() {return this->mesh->C[this->n().n().o().GetId()];};

  /// Renvoie l'identifiant du coin.
  unsigned int GetId() {return this->id;};
  /// Renvoie la mesh a laquelle appartient ce coin.
  inline wlMesh *GetMesh() {return this->mesh;};

  /// Teste si 2 coins sont equivalents.
  bool operator==(wlCoin c) {return c.GetId() == this->id;};
  /// Teste si 2 coins sont differents.
  bool operator!=(wlCoin c) {return c.GetId() != this->id;};

private:
  // la mesh
  wlMesh *mesh;
  // le numero du coin, ie son indice dans la table des coins.
  unsigned int id;
};

#endif
