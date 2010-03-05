/**
 *  \file ListGroupnameContainer.h    \brief Store a list of Classnames
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2010 Sali Lab. All rights reserved.
 */

#ifndef IMPCORE_INTERNAL_CORE_LIST_GROUPNAME_CONTAINER_H
#define IMPCORE_INTERNAL_CORE_LIST_GROUPNAME_CONTAINER_H

#include "../core_config.h"
#include <IMP/GroupnameContainer.h>
#include <IMP/internal/container_helpers.h>
#include "groupname_helpers.h"
#include <IMP/ScoreState.h>

IMPCORE_BEGIN_INTERNAL_NAMESPACE


class IMPCOREEXPORT CoreListGroupnameContainer:
public internal::ListLikeGroupnameContainer
{
  IMP_ACTIVE_CONTAINER_DECL(CoreListGroupnameContainer);
public:
  CoreListGroupnameContainer(std::string name);
  CoreListGroupnameContainer(const char *name);
  CoreListGroupnameContainer(bool);
  void add_classname(PassValue vt);
  void add_classnames(const ClassnamesTemp &c);
  void set_classnames(ClassnamesTemp c);
  void clear_classnames();
  IMP_LISTLIKE_GROUPNAME_CONTAINER(CoreListGroupnameContainer);
};


IMPCORE_END_INTERNAL_NAMESPACE

#endif  /* IMPCORE_INTERNAL_CORE_LIST_GROUPNAME_CONTAINER_H */
