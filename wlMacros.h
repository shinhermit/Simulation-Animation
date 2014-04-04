///////////////////////////////////////////////////////////////////////////////
//  File         :    wlMacros.h                                             //
//  Author       :    Wilfrid Lefer                                          //
//  Comment      :    A set of convenient macros for C++ developments        //
//                    Be careful: some private variables must be initialized //
//                    in the constructor of the class!                       //
//                    Note: this is generally available through a simple     //
//                    to the Init() method associated to the variable.       //
//  Related file :    None                                                   //
//                    Wilfrid Lefer - UPPA - 2003                            //
///////////////////////////////////////////////////////////////////////////////


#ifndef _WL_MACROS_H
#define _WL_MACROS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

// Qt macros ayant pour objectif de declarer en une seul fois :
//   - une variable privee d'un type simple,
//   - une paires de fonctions de type SetGet avec un argument du type en question,
//   - une fonction Set avec un type QString, qui pourra etre connectee au signal Qt d'un interacteur.
#define wlSetIntSlotMacro(name, arg) \
  private: \
    int arg; \
  public slots: \
    void Set##name(QString arg) {this->Set##name(arg.toInt());}; \
    void Set##name(int arg) {this->arg = arg;}; \
    int Get##name() {return this->arg;}
#define wlSetUnsignedIntSlotMacro(name, arg) \
  private: \
    unsigned int arg; \
  public slots: \
    void Set##name(QString arg) {this->Set##name(arg.toInt());}; \
    void Set##name(unsigned int arg) {this->arg = arg;}; \
    unsigned int Get##name() {return this->arg;}
#define wlSetFloatSlotMacro(name, arg) \
  private: \
    float arg; \
  public slots: \
    void Set##name(QString arg) {this->Set##name(arg.toFloat());}; \
    void Set##name(float arg) {this->arg = arg;}; \
    float Get##name() {return this->arg;}
#define wlSetDoubleSlotMacro(name, arg) \
  private: \
    double arg; \
  public slots: \
    void Set##name(QString arg) {this->Set##name(arg.toDouble());}; \
    void Set##name(double arg) {this->arg = arg;}; \
    double Get##name() {return this->arg;}

// SetGet macros
#define wlGetStringMacro(name, size) \
private: \
  char name[size+1]; \
  void Init##name() { \
    this->name[0] = '\0'; \
  }; \
  int Set##name(char *value) { \
    if (value==NULL || strlen(value)>size) { \
      this->Error("pointeur nul ou nom de fichier trop long"); \
      return 1; \
    } else { \
      strncpy(this->name, value, size); \
      return 0; \
    } \
  }; \
public: \
  int Is##name() { \
    return this->name[0] != '\0'; \
  }; \
  char *Get##name() { \
    return this->name; \
  }; \
  void Get##name(char *var) { \
    strcpy(var, this->name); \
  };

#define wlSetGetStringMacro(name, size) \
private: \
  char name[size+1]; \
public: \
  void Init##name() { \
    this->name[0] = '\0'; \
  }; \
  int Is##name() { \
    return this->name[0] != '\0'; \
  }; \
  int Set##name(char *value) { \
    if (value==NULL || strlen(value)>size) { \
      this->Error("pointeur nul ou nom de fichier trop long"); \
      return 1; \
    } else { \
      strncpy(this->name, value, size); \
      return 0; \
    } \
  }; \
  char *Get##name() { \
    return this->name; \
  }; \
  void Get##name(char *var) { \
    strcpy(var, this->name); \
  };

#define wlGetBooleanMacro(name) \
private: \
  int name; \
  void Set##name(int value) { \
    this->name = value!=0 ? 1 : 0; \
  }; \
  void name ##On() { \
    this->name = 1; \
  }; \
  void name ##Off() { \
    this->name = 0; \
  }; \
public: \
  int Get##name() { \
    return this->name; \
  }; \
  void Get##name(int *var) { \
    *var = this->name; \
  };

#define wlSetGetBooleanMacro(name) \
private: \
  int name; \
public: \
  void Set##name(int value) { \
    this->name = value!=0 ? 1 : 0; \
  }; \
  int Get##name() { \
    return this->name; \
  }; \
  void Get##name(int *var) { \
    *var = this->name; \
  }; \
  void name ##On() { \
    this->name = 1; \
  }; \
  void name ##Off() { \
    this->name = 0; \
  };

#define wlGetMacro(name, type) \
private: \
  type name; \
  void Set##name(type value) { \
    this->name = value; \
  }; \
public: \
  type Get##name() { \
    return this->name; \
  }; \
  void Get##name(type *var) { \
    *var = this->name; \
  };

#define wlSetGetMacro(name, type) \
private: \
  type name; \
public: \
  void Set##name(type value) { \
    this->name = value; \
  }; \
  type Get##name() { \
    return this->name; \
  }; \
  void Get##name(type *var) { \
    *var = this->name; \
  };

#define wlSetGetClampMacro(name, type, min, max) \
private: \
  type name; \
public: \
  void Set##name(type value) { \
    this->name = value<min ? min : value>max ? max : value; \
  }; \
  type Get##name() { \
    return this->name; \
  }; \
  void Get##name(type *var) { \
    *var = this->name; \
  }; \
  type GetMin##name() { \
    return min; \
  }; \
  void GetMin##name(type *var) { \
    *var = min; \
  }; \
  type GetMax##name() { \
    return max; \
  }; \
  void GetMax##name(type *var) { \
    *var = max; \
  };

#define wlGetDoubleMacro(name, name1, name2, type) \
private: \
  type name[2]; \
  void Set##name(type name1, type name2) { \
    this->name[0] = name1; \
    this->name[1] = name2; \
  }; \
  void Set##name(type *name) { \
    this->name[0] = name[0]; \
    this->name[1] = name[1]; \
  }; \
  void Set##name1(type name1) { \
    this->name[0] = name1; \
  }; \
  void Set##name2(type name2) { \
    this->name[1] = name2; \
  }; \
public: \
  type *Get##name() { \
    return this->name; \
  }; \
  void Get##name(type *var) { \
    var[0] = this->name[0]; \
    var[1] = this->name[1]; \
  }; \
  void Get##name(type *var1, type *var2) { \
    *var1 = this->name[0]; \
    *var2 = this->name[1]; \
  }; \
  type Get##name1() { \
    return this->name[0]; \
  }; \
  type Get##name2() { \
    return this->name[1]; \
  };

#define wlSetGetDoubleMacro(name, name1, name2, type) \
private: \
  type name[2]; \
public: \
  void Set##name(type name1, type name2) { \
    this->name[0] = name1; \
    this->name[1] = name2; \
  }; \
  void Set##name(type *name) { \
    this->name[0] = name[0]; \
    this->name[1] = name[1]; \
  }; \
  void Set##name1(type name1) { \
    this->name[0] = name1; \
  }; \
  void Set##name2(type name2) { \
    this->name[1] = name2; \
  }; \
  type *Get##name() { \
    return this->name; \
  }; \
  void Get##name(type *var) { \
    var[0] = this->name[0]; \
    var[1] = this->name[1]; \
  }; \
  void Get##name(type *var1, type *var2) { \
    *var1 = this->name[0]; \
    *var2 = this->name[1]; \
  }; \
  type Get##name1() { \
    return this->name[0]; \
  }; \
  type Get##name2() { \
    return this->name[1]; \
  };

#define wlSetGetDoubleClampMacro(name, name1, name2, type, min, max) \
private: \
  type name[2]; \
public: \
  void Set##name(type name1, type name2) { \
    this->name[0] = name1 < min ? min : name1 > max ? max : name1; \
    this->name[1] = name2 < min ? min : name2 > max ? max : name2; \
  }; \
  void Set##name(type *name) { \
    this->name[0] = name[0] < min ? min : name[0] > max ? max : name[0]; \
    this->name[1] = name[1] < min ? min : name[1] > max ? max : name[1]; \
  }; \
  void Set##name1(type name1) { \
    this->name[0] = name1 < min ? min : name1 > max ? max : name1; \
  }; \
  void Set##name2(type name2) { \
    this->name[1] = name2 < min ? min : name2 > max ? max : name2; \
  }; \
  type *Get##name() { \
    return this->name; \
  }; \
  void Get##name(type *var) { \
    var[0] = this->name[0]; \
    var[1] = this->name[1]; \
  }; \
  void Get##name(type *var1, type *var2) { \
    *var1 = this->name[0]; \
    *var2 = this->name[1]; \
  }; \
  type Get##name1() { \
    return this->name[0]; \
  }; \
  type Get##name2() { \
    return this->name[1]; \
  };

#define wlGetTripleMacro(name, name1, name2, name3, type) \
private: \
  type name[3]; \
  void Set##name(type name1, type name2, type name3) { \
    this->name[0] = name1; \
    this->name[1] = name2; \
    this->name[2] = name3; \
  }; \
  void Set##name(type *name) { \
    this->name[0] = name[0]; \
    this->name[1] = name[1]; \
    this->name[0] = name[2]; \
  }; \
  void Set##name1(type name1) { \
    this->name[0] = name1; \
  }; \
  void Set##name2(type name2) { \
    this->name[1] = name2; \
  }; \
  void Set##name3(type name3) { \
    this->name[2] = name3; \
  }; \
public: \
  type *Get##name() { \
    return this->name; \
  }; \
  void Get##name(type *var) { \
    var[0] = this->name[0]; \
    var[1] = this->name[1]; \
    var[2] = this->name[2]; \
  }; \
  void Get##name(type *var1, type *var2, type *var3) { \
    *var1 = this->name[0]; \
    *var2 = this->name[1]; \
    *var3 = this->name[2]; \
  }; \
  type Get##name1() { \
    return this->name[0]; \
  }; \
  type Get##name2() { \
    return this->name[1]; \
  }; \
  type Get##name3() { \
    return this->name[2]; \
  };

#define wlSetGetTripleMacro(name, name1, name2, name3, type) \
private: \
  type name[3]; \
public: \
  void Set##name(type name1, type name2, type name3) { \
    this->name[0] = name1; \
    this->name[1] = name2; \
    this->name[2] = name3; \
  }; \
  void Set##name(type *name) { \
    this->name[0] = name[0]; \
    this->name[1] = name[1]; \
    this->name[0] = name[2]; \
  }; \
  void Set##name1(type name1) { \
    this->name[0] = name1; \
  }; \
  void Set##name2(type name2) { \
    this->name[1] = name2; \
  }; \
  void Set##name3(type name3) { \
    this->name[2] = name3; \
  }; \
  type *Get##name() { \
    return this->name; \
  }; \
  void Get##name(type *var) { \
    var[0] = this->name[0]; \
    var[1] = this->name[1]; \
    var[2] = this->name[2]; \
  }; \
  void Get##name(type *var1, type *var2, type *var3) { \
    *var1 = this->name[0]; \
    *var2 = this->name[1]; \
    *var3 = this->name[2]; \
  }; \
  type Get##name1() { \
    return this->name[0]; \
  }; \
  type Get##name2() { \
    return this->name[1]; \
  }; \
  type Get##name3() { \
    return this->name[2]; \
  };

#define wlSetGetTripleClampMacro(name, name1, name2, name3, type, min, max) \
private: \
  type name[3]; \
public: \
  void Set##name(type name1, type name2, type name3) { \
    this->name[0] = name1 < min ? min : name1 > max ? max : name1; \
    this->name[1] = name2 < min ? min : name2 > max ? max : name2; \
    this->name[2] = name3 < min ? min : name3 > max ? max : name3; \
  }; \
  void Set##name(type *name) { \
    this->name[0] = name[0] < min ? min : name[0] > max ? max : name[0]; \
    this->name[1] = name[1] < min ? min : name[1] > max ? max : name[1]; \
    this->name[2] = name[2] < min ? min : name[2] > max ? max : name[2]; \
  }; \
  void Set##name1(type name1) { \
    this->name[0] = name1 < min ? min : name1 > max ? max : name1; \
  }; \
  void Set##name2(type name2) { \
    this->name[1] = name2 < min ? min : name2 > max ? max : name2; \
  }; \
  void Set##name3(type name3) { \
    this->name[2] = name3 < min ? min : name3 > max ? max : name3; \
  }; \
  type *Get##name() { \
    return this->name; \
  }; \
  void Get##name(type *var) { \
    var[0] = this->name[0]; \
    var[1] = this->name[1]; \
    var[2] = this->name[2]; \
  }; \
  void Get##name(type *var1, type *var2, type *var3) { \
    *var1 = this->name[0]; \
    *var2 = this->name[1]; \
    *var3 = this->name[2]; \
  }; \
  type Get##name1() { \
    return this->name[0]; \
  }; \
  type Get##name2() { \
    return this->name[1]; \
  }; \
  type Get##name3() { \
    return this->name[2]; \
  };

// Macro for managing sets of any types
// Either Init##plural() or Allocate##plural() can be used to initialize the structure 
#define wlAnySetMacro(name, plural, type) \
private: \
  type **plural; \
  int MaxOf##plural, NumberOf##plural, Current##name; \
  long name##Status; \
public: \
  long Get##name##Status() { \
    return this->name##Status; \
  }; \
  void Init##plural() { \
    this->plural = NULL; \
    this->MaxOf##plural = 0; \
    this->NumberOf##plural = 0; \
    this->Current##name = 0; \
    this->name##Status = 0; \
  }; \
  int Allocate##plural(int n) { \
    if (this->plural != NULL) \
      free(this->plural); \
    if ((this->plural = (type **)malloc(n*sizeof(type *))) == NULL) { \
      this->Error("erreur d'allocation memoire"); \
      return 1; \
    } \
    this->MaxOf##plural = n; \
    this->NumberOf##plural = 0; \
    this->Current##name = 0; \
    this->name##Status++; \
    return 0; \
  }; \
  void Free##plural() { \
    if (this->plural != NULL) { \
      free(this->plural); \
      this->Init##plural(); \
      this->name##Status++; \
    } \
  }; \
  int GetNumberOf##plural() { \
    return this->NumberOf##plural; \
  }; \
  void Init##name##Traversal() { \
    this->Current##name = 0; \
  }; \
  type *GetCurrent##name() { \
    return this->plural[this->Current##name]; \
  }; \
  type *GetNext##name() { \
    return this->plural[this->Current##name++]; \
  }; \
  void GoNext##name() { \
    this->Current##name++; \
  }; \
  type *Get##name(int i) { \
    return this->plural[i]; \
  }; \
  int Add##name(type *item) { \
    if (this->NumberOf##plural == this->MaxOf##plural) { \
      this->MaxOf##plural += 100; \
      if (this->MaxOf##plural == 100 && \
         (this->plural = (type **)malloc(this->MaxOf##plural*sizeof(type *))) == NULL || \
          this->MaxOf##plural != 100 && \
         (this->plural = (type **)realloc(this->plural, this->MaxOf##plural*sizeof(type *))) == NULL) { \
        this->Error("erreur d'allocation memoire"); \
        this->MaxOf##plural -= 100; \
        return 1; \
      } \
    } \
    this->plural[this->NumberOf##plural] = item; \
    this->NumberOf##plural++; \
    this->name##Status++; \
    return 0; \
  }; \

// These definitions are necessary to have a single macro for managing sets of simple types
#define MINshort  SHRT_MIN
#define MAXshort  SHRT_MAX
#define MINint    INT_MIN
#define MAXint    INT_MAX
#define MINlong   LONG_MIN
#define MAXlong   LONG_MAX
#define MINfloat  -FLT_MAX
#define MAXfloat  FLT_MAX
#define MINdouble -DBL_MAX
#define MAXdouble DBL_MAX

// Macro for managing sets of simple types
// Either Init##plural() or Allocate##plural() can be used to initialize the structure 
#define wlSimpleSetMacro(name, plural, type) \
private: \
  type *plural, Min##name, Max##name, Tmp##name; \
  int MaxOf##plural, NumberOf##plural, Current##name, Count##name; \
  long name##Status; \
public: \
  long Get##name##Status() { \
    return this->name##Status; \
  }; \
  void Init##plural() { \
    this->plural = NULL; \
    this->MaxOf##plural = 0; \
    this->NumberOf##plural = 0; \
    this->Current##name = 0; \
    this->Min##name = MAX##type; \
    this->Max##name = MIN##type; \
    this->name##Status = 0; \
  }; \
  int Allocate##plural(int n) { \
    if (this->plural != NULL) \
      free(this->plural); \
    if ((this->plural = (type *)malloc(n*sizeof(type))) == NULL) { \
      this->Error("erreur d'allocation memoire"); \
      return 1; \
    } \
    this->MaxOf##plural = n; \
    this->NumberOf##plural = 0; \
    this->Current##name = 0; \
    this->Min##name = MAX##type; \
    this->Max##name = MIN##type; \
    this->name##Status++; \
    return 0; \
  }; \
  int Copy##plural(type *ptr, int n) { \
    if (this->Allocate##plural(n) != 0) \
      return 1; \
    if (memmove(this->plural, ptr, n*sizeof(type)) == NULL) \
      return 1; \
    this->NumberOf##plural = n; \
    this->ComputeMinMax##name(); \
    this->name##Status++; \
    return 0; \
  }; \
  int Read##plural(FILE *f, int n) { \
    this->Allocate##plural(n); \
    if (fread(this->plural, sizeof(type), n, f) < (size_t)n) \
      return 1; \
    this->NumberOf##plural = n; \
    this->ComputeMinMax##name(); \
    this->name##Status++; \
    return 0; \
  }; \
  void Free##plural() { \
    if (this->plural != NULL) { \
      free(this->plural); \
      this->Init##plural(); \
      this->name##Status++; \
    } \
  }; \
  int GetNumberOf##plural() { \
    return this->NumberOf##plural; \
  }; \
  void Init##name##Traversal() { \
    this->Current##name = 0; \
  }; \
  type GetCurrent##name() { \
    return this->plural[this->Current##name]; \
  }; \
  type GetNext##name() { \
    return this->plural[this->Current##name++]; \
  }; \
  void GoNext##name() { \
    this->Current##name++; \
  }; \
  type Get##name(int i) { \
    return this->plural[i]; \
  }; \
  void Set##name(int i, type val) { \
    this->plural[i] = val; \
    this->name##Status++; \
  }; \
  type *Get##plural##Ptr() const { \
    return this->plural; \
  }; \
  int Add##name(type v) { \
    if (this->NumberOf##plural == this->MaxOf##plural) { \
      this->MaxOf##plural += 100; \
      if (this->MaxOf##plural == 100 && \
         (this->plural = (type *)malloc(this->MaxOf##plural*sizeof(type))) == NULL || \
          this->MaxOf##plural != 100 && \
         (this->plural = (type *)realloc(this->plural, this->MaxOf##plural*sizeof(type))) == NULL) { \
        this->Error("erreur d'allocation memoire"); \
        this->MaxOf##plural -= 100; \
        return 1; \
      } \
    } \
    this->plural[this->NumberOf##plural] = v; \
    this->NumberOf##plural++; \
    this->Min##name = v<this->Min##name ? v : this->Min##name; \
    this->Max##name = v>this->Max##name ? v : this->Max##name; \
    this->name##Status++; \
    return 0; \
  }; \
  void ComputeMinMax##name() { \
    this->Min##name = MAX##type; \
    this->Max##name = MIN##type; \
    for (int i=0 ; i<this->NumberOf##plural ; i++) { \
      this->Min##name = this->plural[i]<this->Min##name ? this->plural[i] : this->Min##name; \
      this->Max##name = this->plural[i]>this->Max##name ? this->plural[i] : this->Max##name; \
    } \
  }; \
  type GetMin##name() { \
    return this->Min##name; \
  }; \
  type GetMax##name() { \
    return this->Max##name; \
  }; \
  void Clamp##name(type min, type max) { \
    for (this->Count##name=this->GetNumberOf##plural()-1 ; this->Count##name>=0 ; this->Count##name--) { \
      this->plural[this->Count##name] = this->plural[this->Count##name]<min ? min : this->plural[this->Count##name]; \
      this->plural[this->Count##name] = this->plural[this->Count##name]>max ? max : this->plural[this->Count##name]; \
    } \
    this->name##Status++; \
  }; \
  type GetSum##name() { \
    for (this->Count##name=this->GetNumberOf##plural()-1, this->Tmp##name=0 ; \
         this->Count##name>=0 ; \
         this->Count##name--) \
      this->Tmp##name += this->plural[this->Count##name]; \
    return this->Tmp##name; \
  }; \
  float GetAverage##name() { \
    return this->GetSum##name() / (float)this->GetNumberOf##plural(); \
  }; \
  void Shift##plural(type offset) { \
    for (this->Count##name=this->GetNumberOf##plural()-1 ; this->Count##name>=0 ; this->Count##name--) \
      this->plural[this->Count##name] += offset; \
    this->name##Status++; \
  }; \
  void Scale##plural(type factor) { \
    for (this->Count##name=this->GetNumberOf##plural()-1 ; this->Count##name>=0 ; this->Count##name--) \
      this->plural[this->Count##name] *= factor; \
    this->name##Status++; \
  }; \
  void Print##plural() { \
    printf("plural: "); \
    for (this->Count##name=0 ; this->Count##name<this->GetNumberOf##plural() ; this->Count##name++) \
      printf("%f ", (float)this->plural[this->Count##name]); \
    printf("\n"); \
  };

// Macro for managing structured sets of values
#define wlStructuredSetMacro(name, plural) \
private: \
  int NumberOf##plural, Min##name, Max##name, Incr##name, Current##name; \
  long name##Status; \
public: \
  long Get##name##Status() { \
    return this->name##Status; \
  }; \
  void Init##name() { \
    this->NumberOf##plural = 0; \
    this->Current##name = 0; \
    this->name##Status = 0; \
  }; \
  int SetMinMaxIncr##name(int min, int max, int incr) { \
    this->Min##name = min; \
    this->Max##name = max; \
    this->Incr##name = incr; \
    if (incr == 0) \
      if (min == max) \
        this->NumberOf##plural = 1; \
      else { \
        this->Error("definition d'un intervalle non valide : min=%d max=%d increment=%d", min, max, incr); \
        return 1; \
      } \
    else this->NumberOf##plural = (max - min) / incr + 1; \
    this->name##Status++; \
    return 0; \
  }; \
  int GetNumberOf##plural() { \
    return this->NumberOf##plural; \
  }; \
  void Init##name##Traversal() { \
    this->Current##name = this->Min##name; \
  }; \
  int GetCurrent##name() { \
    return this->Current##name; \
  }; \
  int GetNext##name() { \
    this->Current##name += this->Incr##name; \
    return this->Current##name - this->Incr##name; \
  }; \
  void GoNext##name() { \
    this->Current##name += this->Incr##name; \
  }; \
  int Get##name(int i) { \
    return this->Min##name + i * this->Incr##name; \
  }; \
  int GetMin##name() { \
    return this->Min##name; \
  }; \
  int GetMax##name() { \
    return this->Max##name; \
  }; \
  int GetIncr##name() { \
    return this->Incr##name; \
  }; \
  void Clamp##name(int min, int max) { \
    if (this->Min##name < min) \
      if ((min - this->Min##name) % this->Incr##name > 0) \
        this->Min##name += ((min - this->Min##name) / this->Incr##name + 1) * this->Incr##name; \
      else \
        this->Min##name = min; \
    if (this->Max##name > max) \
      if ((this->Max##name - max) % this->Incr##name > 0) \
        this->Max##name -= ((this->Max##name - max) / this->Incr##name + 1) * this->Incr##name; \
      else \
        this->Max##name = max; \
    this->NumberOf##plural = (this->Max##name - this->Min##name) / this->Incr##name + 1; \
    this->name##Status++; \
  }; \
  float GetSum##name() { \
    return (this->Max##name + this->Min##name) / 2.0 * this->NumberOf##plural; \
  }; \
  float GetAverage##name() { \
    return (this->Max##name + this->Min##name) / 2.0; \
  }; \
  void Shift##plural(int offset) { \
    this->Min##name += offset; \
    this->Max##name += offset; \
    this->name##Status++; \
  }; \
  void Scale##plural(int factor) { \
    this->Min##name *= factor; \
    this->Max##name *= factor; \
    this->Incr##name *= factor; \
    this->name##Status++; \
  }; \
  void Print##plural() { \
    printf("plural: min=%d max=%d incr=%d\n", this->Min##name, this->Max##name, this->Incr##name); \
  };

// Export macros: export SetGet methods of composed classes
#define wlExportGetStringMacro(object, name) \
  int Is##name() { \
    return this->object->Is##name(); \
  }; \
  char *Get##name() { \
    return this->object->Get##name(); \
  }; \
  void Get##name(char *var) { \
    this->object->Get##name(var); \
  };

#define wlExportSetGetStringMacro(object, name) \
  void Init##name() { \
    this->object->Init##name(); \
  }; \
  int Is##name() { \
    return this->object->Is##name(); \
  }; \
  void Set##name(char *value) { \
    this->object->Set##name(value); \
  }; \
  char *Get##name() { \
    return this->object->Get##name(); \
  }; \
  void Get##name(char *var) { \
    this->object->Get##name(var); \
  };

#define wlExportGetBooleanMacro(object, name) \
  int Get##name() { \
    return this->object->Get##name(); \
  }; \
  void Get##name(int *var) { \
    this->object->Get##name(var); \
  };

#define wlExportSetGetBooleanMacro(object, name) \
  void Set##name(int value) { \
    this->object->Set##name(value); \
  }; \
  int Get##name() { \
    return this->object->Get##name(); \
  }; \
  void Get##name(int *var) { \
    this->object->Get##name(var); \
  }; \
  void name ##On() { \
    this->object->name ##On(); \
  }; \
  void name ##Off() { \
    this->object->name ##Off(); \
  };

#define wlExportGetMacro(object, name, type) \
  type Get##name() { \
    return this->object->Get##name(); \
  }; \
  void Get##name(type *var) { \
    this->object->Get##name(var); \
  };

#define wlExportSetGetMacro(object, name, type) \
  void Set##name(type value) { \
    this->object->Set##name(value); \
  }; \
  type Get##name() { \
    return this->object->Get##name(); \
  }; \
  void Get##name(type *var) { \
    this->object->Get##name(var); \
  };

#define wlExportSetGetClampMacro(object, name, type, min, max) \
  void Set##name(type value) { \
    this->object->Set##name(value); \
  }; \
  type Get##name() { \
    return this->object->Get##name(); \
  }; \
  void Get##name(type *var) { \
    this->object->Get##name(var); \
  }; \
  type GetMin##name() { \
    return this->object->GetMin##name(); \
  }; \
  void GetMin##name(type *var) { \
    this->object->GetMin##name(var); \
  }; \
  type GetMax##name() { \
    return this->object->GetMax##name(); \
  }; \
  void GetMax##name(type *var) { \
    this->object->GetMax##name(var); \
  };

#define wlExportGetDoubleMacro(object, name, name1, name2, type) \
  type *Get##name() { \
    return this->object->Get##name(); \
  }; \
  void Get##name(type *var) { \
    this->object->Get##name(var); \
  }; \
  void Get##name(type *var1, type *var2) { \
    this->object->Get##name(var1, var2); \
  }; \
  type Get##name1() { \
    return this->object->Get##name1(); \
  }; \
  type Get##name2() { \
    return this->object->Get##name2(); \
  };

#define wlExportSetGetDoubleMacro(object, name, name1, name2, type) \
  void Set##name(type name1, type name2) { \
    this->object->Set##name(name1, name2); \
  }; \
  void Set##name(type *name) { \
    this->object->Set##name(name); \
  }; \
  void Set##name1(type name1) { \
    this->object->Set##name(name1); \
  }; \
  void Set##name2(type name2) { \
    this->object->Set##name(name2); \
  }; \
  type *Get##name() { \
    return this->object->Get##name(); \
  }; \
  void Get##name(type *var) { \
    this->object->Get##name(var); \
  }; \
  void Get##name(type *var1, type *var2) { \
    this->object->Get##name(var1, var2); \
  }; \
  type Get##name1() { \
    return this->object->Get##name1(); \
  }; \
  type Get##name2() { \
    return this->object->Get##name2(); \
  };

#define wlExportGetTripleMacro(object, name, name1, name2, name3, type) \
  type *Get##name() { \
    return this->object->Get##name(); \
  }; \
  void Get##name(type *var) { \
    this->object->Get##name(var); \
  }; \
  void Get##name(type *var1, type *var2, type *var3) { \
    this->object->Get##name(var1, var2, var3); \
  }; \
  type Get##name1() { \
    return this->object->Get##name1(); \
  }; \
  type Get##name2() { \
    return this->object->Get##name2(); \
  }; \
  type Get##name3() { \
    return this->object->Get##name3(); \
  };

#define wlExportSetGetTripleMacro(object, name, name1, name2, name3, type) \
  void Set##name(type name1, type name2, type name3) { \
    this->object->Set##name(name1, name2, name3); \
  }; \
  void Set##name(type *name) { \
    this->object->Set##name(name); \
  }; \
  void Set##name1(type name1) { \
    this->object->Set##name(name1); \
  }; \
  void Set##name2(type name2) { \
    this->object->Set##name(name2); \
  }; \
  void Set##name3(type name3) { \
    this->object->Set##name(name3); \
  }; \
  type *Get##name() { \
    return this->object->Get##name(); \
  }; \
  void Get##name(type *var) { \
    this->object->Get##name(var); \
  }; \
  void Get##name(type *var1, type *var2, type *var3) { \
    this->object->Get##name(var1, var2, var3); \
  }; \
  type Get##name1() { \
    return this->object->Get##name1(); \
  }; \
  type Get##name2() { \
    return this->object->Get##name2(); \
  }; \
  type Get##name3() { \
    return this->object->Get##name3(); \
  };

#define wlExportStructuredSetMacro(object, name, plural) \
public: \
  int GetNumberOf##plural() { \
    return this->object->GetNumberOf##plural(); \
  }; \
  void Init##name##Traversal() { \
    this->object->Init##name##Traversal(); \
  }; \
  int GetCurrent##name() { \
    return this->object->GetCurrent##name(); \
  }; \
  int GetNext##name() { \
    return this->object->GetNext##name(); \
  }; \
  void GoNext##name() { \
    this->object->GoNext##name(); \
  }; \
  int GetMin##name() { \
    return this->object->GetMin##name(); \
  }; \
  int GetMax##name() { \
    return this->object->GetMax##name(); \
  }; \
  int GetIncr##name() { \
    return this->object->GetIncr##name(); \
  };

#define wlExportAnySetMacro(object, name, plural, type) \
public: \
  int GetNumberOf##plural() { \
    return this->object->GetNumberOf##plural(); \
  }; \
  void Init##name##Traversal() { \
    this->object->Init##name##Traversal(); \
  }; \
  type *GetCurrent##name() { \
    return this->object->GetCurrent##name(); \
  }; \
  type *GetNext##name() { \
    return this->object->GetNext##name(); \
  }; \
  void GoNext##name() { \
    this->object->GoNext##name(); \
  }; \
  type Get##name(int i) { \
    return this->object->Get##name(i); \
  };

#define wlExportSimpleSetMacro(object, name, plural, type) \
public: \
  int GetNumberOf##plural() { \
    return this->object->GetNumberOf##plural(); \
  }; \
  void Init##name##Traversal() { \
    this->object->Init##name##Traversal(); \
  }; \
  type *GetCurrent##name() { \
    return this->object->GetCurrent##name(); \
  }; \
  type *GetNext##name() { \
    return this->object->GetNext##name(); \
  }; \
  void GoNext##name() { \
    this->object->GoNext##name(); \
  }; \
  type Get##name(int i) { \
    return this->object->Get##name(i); \
  }; \
  type *Get##plural##Ptr() { \
    return this->object->Get##plural##Ptr(); \
  }; \
  void ComputeMinMax##name() { \
    this->object->ComputeMinMax##name(); \
  }; \
  type GetMin##name() { \
    return this->object->GetMin##name(); \
  }; \
  type GetMax##name() { \
    return this->object->GetMax##name(); \
  };

#endif
