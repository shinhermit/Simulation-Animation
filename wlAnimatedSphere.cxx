///////////////////////////////////////////////////////////////////////////////
//  File         :    wlAnimatedSphere.cxx                                   //
//  Author       :    Wilfrid LEFER                                          //
//  Comment      :    Modele de sphere animee                                //
//  Related file :    wlAnimatedSphere.h, wlMesh.{h,cxx}                     //
//                    Wilfrid Lefer - UPPA - 2010                            //
///////////////////////////////////////////////////////////////////////////////


#include <wlAnimatedSphere.h>

wlAnimatedSphere::wlAnimatedSphere(int debug, wlQGLViewer *v, QString filename)
  : wlAnimatedMesh(debug, v, filename)
{
  this->Trace("-> wlAnimatedSphere(%p, \"%s\")", v, filename.toAscii().constData());
  this->Clear();
  this->Trace("<- wlAnimatedSphere()");
}

void
wlAnimatedSphere::PrintSelf()
{
  wlAnimatedMesh::PrintSelf();
}

void
wlAnimatedSphere::PrintContent()
{
  this->PrintSelf();
}

float
wlAnimatedSphere::GetRadius()
{
  // cet appel a GetSize() a pour objectif de verifier que la donnee est a jour
  (void)this->GetSize();
  this->radius = this->size[0] / 2;
  return this->radius;
}

QVector<double>
wlAnimatedSphere::ComputeCollisionWithPlan(QVector<double> equation)
{
    return wlAnimatedMesh::ComputeCollisionWithPlan(equation);
//    QVector<double> center(this->GetCurrentCenter());
//    QVector<double> out, P0, P1;
//    // on calcule l'equation cartesienne du plan
//    double a = equation[3];
//    double b = equation[4];
//    double c = equation[5];
//    double d = - (a * equation[0] + b * equation[1] + c * equation[2]);
//    // on verifie que la sphere etait bien du "bon cote" du plan au debut du pas de temps afin d'eviter de detecter une collision
//    // entre l'arriere de la sphere et le plan, autrement dit apres que la sphere soit passee a travers le plan
//    // pour cele on verifie que le centre de la sphere est reste du meme cote du plan au debut et a la fin du pas de temps
//    // on injecte les coordonnees du centre de la sphere dans l'equation cartesienne du plan pour les 2 instants et on verifie que le signe est identique
//    // si ce n'est pas le cas, on en deduit que la collision a ete detectee trop tard
//    P0 << this->position[0] << this->position[1] << this->position[2];
//    P1 = P0;
//    P0 += Tmat_p;
//    P1 += Tmat;
//    double side1 = a * P0[0] + b * P0[1] + c * P0[2] + d;
//    double side2 = a * P1[0] + b * P1[1] + c * P1[2] + d;

//    if (side1 * side2 <= 0) {
//      out << this->tolerance * 2;
//      return out;
//    }
//    // on verifie que la sphere se dirige bien en direction du plan afin d'eviter de detecter une collision avec le plan apres rebond
//    // pour cela on verifie que le signe du produit scalaire entre vecteur vitesse et normale au plan est le meme au debut et a la fin du pas de temps
//    // si ce n'est pas le cas, on en deduit qu'il ne peut y avoir collision
//    double ps1 = this->cvel_p[0] * equation[3] + this->cvel_p[1] * equation[4] + this->cvel_p[2] * equation[5];
//    double ps2 = this->cvel[0] * equation[3] + this->cvel[1] * equation[4] + this->cvel[2] * equation[5];
//    if (ps1 * ps2 <= 0)
//      return QVector<double>();
//    // la norme du vecteur normal au plan
//    double norme = sqrt(equation[3] * equation[3] + equation[4] * equation[4] + equation[5] * equation[5]);
//    // la distance de la sphere au plan
//    double dist = fabs(equation[3] * (center[0] - equation[0]) + equation[4] * (center[1] - equation[1]) +
//            equation[5] * (center[2] - equation[2])) / norme - this->GetRadius();
//    // le point d'intersection
//    out = equation;
//    out[0] = center[0] - dist / norme * equation[0];
//    out[1] = center[1] - dist / norme * equation[1];
//    out[2] = center[2] - dist / norme * equation[2];
//    if (dist > this->tolerance)
//      return QVector<double>();
//    if (dist < -this->tolerance) {
//      out << -dist;
//      return out;
//    }

//    return out;
}

QVector<double>
wlAnimatedSphere::ComputeCollisionWithOther(wlAnimatedMesh *other)
{
    wlAnimatedMesh::ComputeCollisionWithOther(other);
    if (!other->inherits("wlAnimatedSphere")) {
      this->Error("ComputeCollision: l'autre mesh n'est pas du type \"wlAnimatedSphere\"");
      return QVector<double>();
    }
    QVector<double> out;
    out << (other->GetCurrentCenter()[0] - this->GetCurrentCenter()[0]);
    out << (other->GetCurrentCenter()[1] - this->GetCurrentCenter()[1]);
    out << (other->GetCurrentCenter()[2] - this->GetCurrentCenter()[2]);
    // au prealable on verifie que les objets ne sont pas en train de s'eloigner les uns des autres
    // ce qui peut arriver par exemple juste apres une collision si le pas de temps restant est court
    double ps1 = out[0] * this->GetVelocity()[0] + out[1] * this->GetVelocity()[1] + out[2] * this->GetVelocity()[2];
    double ps2 = out[0] * other->GetVelocity()[0] + out[1] * other->GetVelocity()[1] + out[2] * other->GetVelocity()[2];
    if (ps1<=0 && ps2>=0)
      return QVector<double>();
    // on calcule maintenant le point d'intersection
    double norm = sqrt(out[0] * out[0] + out[1] * out[1] + out[2] * out[2]);
    double d = norm - (this->GetRadius() + other->GetRadius());
    this->Trace("out = %lf %lf %lf norm = %lf d = %lf rayons = %lf %lf", out[0], out[1], out[2], norm, d, this->GetRadius(), other->GetRadius());
    this->Trace("size = %lf %lf %lf", this->GetSize()[0], this->GetSize()[1], this->GetSize()[2]);
    if (d > this->tolerance)
      return QVector<double>();
    if (d < -this->tolerance) {
      out.resize(1);
      out[0] = -d;
      return out;
    }
    out << out[0] / norm;
    out << out[1] / norm;
    out << out[2] / norm;
    out[0] = out[0] / 2 + this->GetCurrentCenter()[0];
    out[1] = out[1] / 2 + this->GetCurrentCenter()[1];
    out[2] = out[2] / 2 + this->GetCurrentCenter()[2];
    return out;
}

void
wlAnimatedSphere::ComputeReaction(QVector<double> impact)
{
  return wlAnimatedMesh::ComputeReaction(impact);
}
