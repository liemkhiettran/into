/* This file is part of Into. 
 * Copyright (C) Intopii 2013.
 * All rights reserved.
 *
 * Licensees holding a commercial Into license may use this file in
 * accordance with the commercial license agreement. Please see
 * LICENSE.commercial for commercial licensing terms.
 *
 * Alternatively, this file may be used under the terms of the GNU
 * Affero General Public License version 3 as published by the Free
 * Software Foundation. In addition, Intopii gives you special rights
 * to use Into as a part of open source software projects. Please
 * refer to LICENSE.AGPL3 for details.
 */

#ifndef _PIIGENERICBINARYINPUTARCHIVE_H
#define _PIIGENERICBINARYINPUTARCHIVE_H

/** @file */

#include "PiiBinaryInputArchive.h"
#include "PiiGenericInputArchive.h"

/**
 * A shorthand for a binary input archive derived from
 * PiiGenericInputArchive.
 *
 * @ingroup Serialization
 */
typedef PiiGenericInputArchive::Impl<PiiBinaryInputArchive> PiiGenericBinaryInputArchive;

#endif //_PIIGENERICBINARYINPUTARCHIVE_H