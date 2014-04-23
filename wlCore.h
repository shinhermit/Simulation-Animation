///////////////////////////////////////////////////////////////////////////////
//  File         :    wlCore.h                                               //
//  Author       :    Wilfrid Lefer                                          //
//  Comment      :    Core classe pour mes developpements                    //
//  Related file :    wlCore.cxx                                             //
//                    Wilfrid Lefer - UPPA - 2003                            //
///////////////////////////////////////////////////////////////////////////////


#ifndef _WL_CORE_H
#define _WL_CORE_H

#include <wlMacros.h>
#include <limits.h>
#include <stdarg.h>
#include <map>

#define MAXSTR 1000

/** \brief Classe de base pour les developpements de son auteur.
    Cette classe fournit :
    <ul>
      <li>le mode Debug,</li>
      <li>les fonctions d'impression donnant le nom de la classe qui ecrit,</li>
      <li>une bascule permettant de savoir si le resultat de la classe
          est a jour par rapport a ses donnees d'entree.</li>
    </ul>
*/
class wlCore
{
public:
  /// Instancie un nouvel objet dans le mode approprie.
  wlCore(int debug=0);
  /// Destructeur.
  virtual ~wlCore() {}
  /// Cette methode devra etre surchargee par tous les objets heritant, directement ou indirectement, de wlCore.
  virtual char *GetClassName() {return "wlCore";}

  /// Les methodes suivantes permettent de basculer entre les modes debug et normal.
  wlSetGetBooleanMacro(Debug)

  /// Imprime un message en precedant celui-ci du nom de la classe.
  void Print(char *format, ...);
  /// Imprime un message d'attention en precedant celui-ci du nom de la classe.
  void Warning(char *format, ...);
  /// Imprime un message d'erreur en precedant celui-ci du nom de la classe.
  void Error(char *format, ...);
  /// Idem que Print() mais le message n'est imprime que si l'objet est dans le mode debug.
  void Trace(char *format, ...);

  /// Place la variable d'etat v dans l'etat ou sa sortie n'est plus a jour par rapport a son entree.
  inline void Modified(const char *v) {this->modified[v] = 1;}
  /// Place la variable d'etat v dans l'etat ou sa sortie est a jour par rapport a son entree.
  inline void Updated(const char *v) {this->modified[v] = 0;}
  /// Renvoie vrai si la sortie associee a la variable d'etat v est a jour par rapport a son entree.
  inline unsigned int UpToDate(const char *v) {return !this->modified[v];}

private:
  int maxline;
  char *line;
  std::map<const char *,unsigned int> modified;
};

#endif
