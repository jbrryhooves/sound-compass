/**
 * \file
 *
 * \brief
 *
 * \author Josh Berry
 * \date
 */

#ifndef ASSERT_H_
#define ASSERT_H_

#include <util/error.hpp>

//=====================
//      DEFINES
//=====================

// Implement our own ASSERT macro
#define ASSERT(expr)  ((expr) ? (void)0 : error_fatal(fseAssert, (const char*)__BASE_FILE__, __LINE__))

// Implement an assert than can be used at compile time
#define STATIC_ASSERT(cond, msg) typedef char static_assertion_##msg[(!!(cond))*2-1]

//=========
//  Types
//=========

//=============
//  Interface
//=============

#endif /* ASSERT_H_ */
