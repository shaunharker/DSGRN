/// sqlambda.h
//    Shaun Harker, 2015-05-18
//    A C++ interface to SQLite
//
// The MIT License (MIT)
//
// Copyright (c) 2015 Shaun Harker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// Documentation:
//   The code attempts to be "self-documenting"
#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <functional>
#include <stdexcept>
#include <typeinfo>
#include "sqlite3.h"

namespace sqlite {

// Forward Declarations
class database;
class statement;
class statement_wrapper;
class database_wrapper;

/// class database
///   This class is used to 
///   (1) Create connections to a database (database constructor)
///   (2) Create prepared SQL statements (prepare)
///   (3) Execute SQL directives (exec)
class database {
public:

  /// database
  ///   Default constructor
  ///   Note: There is no error checking yet to 
  ///         make sure the object is intialized 
  ///         before using! (TODO)
  database ( void );

  /// database [API]
  ///   Open a database connection
  database ( std::string const & db_name );

  /// prepare [API]
  ///   Prepare a statement with sql_expr
  statement 
  prepare ( std::string const& sql_expr ) const;

  /// exec [API]
  ///   Immediately execute sql_expr
  ///   Equivalent to "prepare(sql_expr).exec()"
  void 
  exec ( std::string const& sql_expr ) const;

  /// sqlite3* cast [API]
  ///   Return underlying sqlite3* pointer
  ///   This is used to access the SQLite C API
  operator sqlite3* () const;

  /// open [API]
  ///   Open a new database connection
  void
  open ( std::string const & db_name );

  /// close [API]
  ///   Close the database
  ///   Note: if other copies of the database exist,
  ///         the database will not close until they
  ///         call close or go out of scope.
  void
  close ( void );

private:
  std::shared_ptr<database_wrapper>  db_;
};
/// class statement
///   This class is used to handle prepared statements.
///   Prepared statements are useful for the sake of 
///   efficiency when doing the same operation many times
///   except with different data. Two main examples are
///   insertion of many rows into a table, and reading
///   the rows from the result of a "select" operation.
///   When reading rows, a forEach method is provided
///   which evaluates the user-supplied callback function
///   on each row.
class statement {
public:
  /// bind [API]
  ///   Bind data to a prepared statement.
  template<typename... Args> statement& 
  bind ( Args... args );

  /// exec [API]
  ///   Execute the statement
  statement& 
  exec ( void );

  /// forEach [API]
  ///   Execute the statement and call 
  ///   "callback" on each resulting row
  template<typename F> statement& 
  forEach ( F const& callback );

  /// sqlite3_stmt* cast [API]
  ///   Return underlying sqlite3_stmt* pointer
  ///   This is used to access the SQLite C API
  operator sqlite3_stmt* () const;

  /// statement
  ///   Default constructor, to allow:
  ///     statement prepped;
  ///     prepped = db . prepare ( "..." );
  ///   Note: There is no error checking yet to 
  ///         make sure the object is initialized 
  ///         before using! (TODO)
  statement ( void );

private:
	friend class database;
	/// statement::statement [IMPLEMENTATION]
	///   Construct a statement. Do not call directly.
	///   It is meant to only be called by the "database::statement" 
	///   method
	statement(database_wrapper const& db, std::string const& sql_expr);

	/// reset [IMPLEMENTATION]
	///   Reset a prepared statement and clear its bindings 
	statement& _reset ( void );

	/// _bind_one [IMPLEMENTATION]
	///   Bind a single argument to the next free slot
	template < typename T > statement& 
	_bind_one ( int iCol, T const& t );
	
	/// _bind (terminating case) [IMPLEMENTATION]
	///   No-argument _bind to halt recursion
	statement& 
	_bind ( int iCol );
	
	/// _bind (recursive version) [IMPLEMENTATION]
	///   Variadic template function to bind multiple arguments
	template<typename Head, typename... Tail> statement& 
	_bind ( int iCol, Head head, Tail... tail );
	
	/// _apply (terminating case) [IMPLEMENTATION]
	///   Call the user's callback function
	///   (This is called after all data has been bound to it)
	void 
	_apply(std::function<void()> callback, int i);
	
	/// _apply (recursive version) [IMPLEMENTATION]
	///   Partial function application to bind data to user
	///   callback
	template<typename Head, typename... Tail> void 
	_apply(std::function<void(Head, Tail...)> callback, int i);

	std::shared_ptr<statement_wrapper> stmt_;
};


////////////////////
// IMPLEMENTATION //
////////////////////

/// sql_try(int rc)
///   Throw the exception corresponding to the error code rc
///   (unless rc is not an error code, then do not throw)
int sql_try(int rc);

/// class database_wrapper
///   This class provides a wrapper around sqlite3*
///   It handles initialization in the constructor and
///   finalization in the destructor.
class database_wrapper {
public:
	database_wrapper(std::string const & db_name) {
		sql_try(sqlite3_open(db_name.data(), & db_));
	}

	database_wrapper(sqlite3* db) :  db_(db) { }

	~database_wrapper() {
		sqlite3_close_v2( db_);			
	}

	operator sqlite3* () const {
		return  db_;
	}
private:
	sqlite3 *  db_;
};

/// class statement_wrapper
///   This class provides a wrapper around sqlite3_stmt*
///   It handles initialization in the constructor and
///   finalization in the destructor.
class statement_wrapper {
public:
	statement_wrapper(sqlite3* db, std::string const & sql_expr) {
		sql_try(sqlite3_prepare_v2(db, sql_expr.data(), -1, &stmt_, nullptr));
	}
	~statement_wrapper() {
		sql_try(sqlite3_finalize(stmt_));
	}
	operator sqlite3_stmt* () const {
		return stmt_;
	}
private:
	sqlite3_stmt* stmt_;
};

/// sqlite3_bind
///   This uses C++ features to unify the interface
///   for the various sqlite3_bind_* functions.
template < class T >
int sqlite3_bind(sqlite3_stmt* stmt_, int iCol, T const& t);

/// sqlite3_column
///   This uses C++ features to unify the interface
///   for the various sqlite3_column_* functions.
template < class T >
T sqlite3_column ( sqlite3_stmt* stmt, int iCol );

/// struct sqlite_exception
///   This structure provides error reporting for sqlite
struct sqlite_exception : public std::runtime_error {
	sqlite_exception(std::string const& msg):runtime_error(msg) {}
};

/// We need to use variadic templates, but they don't play well
/// with lambdas unless we can convert them into std::function.
/// The function "FFL" (function-from-lambda) does this.
/// We use the solution due to Nikos Athanasiou from:
/// http://stackoverflow.com/questions/13358672/how-to-convert-a-lambda-to-an-stdfunction-using-templates
template<typename T> struct fun_type {using type = void;};
template<typename Ret, typename Class, typename... Args>
struct fun_type<Ret(Class::*)(Args...) const> {using type = std::function<Ret(Args...)>;};
template<typename F> typename fun_type<decltype(&F::operator())>::type
FFL(F const &func) { return func; }



template < class T >
T sqlite3_column ( sqlite3_stmt* stmt, int iCol ) {
	std::string error = std::string("Unknown column type: ") + 
										  typeid (T).name() + std::string("\n");
	throw sqlite_exception( error );
}
#define SQL_COLUMN(T,CAST,SUFFIX) \
template <> T sqlite3_column<T> ( sqlite3_stmt* stmt, int iCol ) { \
	return T ( (CAST) sqlite3_column_ ## SUFFIX (stmt, iCol) ); }
SQL_COLUMN(int,int,int);
SQL_COLUMN(sqlite3_int64,sqlite3_int64,int64);
SQL_COLUMN(uint64_t,uint64_t,int64);
SQL_COLUMN(std::string,char *,text);
SQL_COLUMN(std::u16string,char16_t *,text16);
SQL_COLUMN(double,double,double);
SQL_COLUMN(float,float,double);

template < class T >
int sqlite3_bind(sqlite3_stmt* stmt_, int iCol, T const& t) {
  std::stringstream ss;
  ss << "Binding failure. Unknown column type for column " 
     << iCol << ". Value is " << t << "\n"; 
	throw sqlite_exception( ss . str () );
}
#define SQL_BIND(T,CAST,SUFFIX) \
template <> int sqlite3_bind<T> ( sqlite3_stmt* stmt_, int iCol, T const& t ) { \
	return sqlite3_bind_ ## SUFFIX (stmt_, iCol, CAST t ); }
#define SQL_BINDTEXT(T,SUFFIX) \
template <> int sqlite3_bind<T> ( sqlite3_stmt* stmt_, int iCol, T const& t ) { \
	return sqlite3_bind_ ## SUFFIX (stmt_, iCol, t.data(), -1, SQLITE_TRANSIENT); }
SQL_BIND(double,,double);
SQL_BIND(float,(double),double);
SQL_BIND(int,,int);
SQL_BIND(uint64_t,(sqlite3_int64),int64);
SQL_BIND(sqlite3_int64,,int64);
SQL_BINDTEXT(std::string,text);
SQL_BINDTEXT(std::u16string,text16);

int sql_try(int rc) {
	if (rc != SQLITE_OK && rc != SQLITE_ROW && rc != SQLITE_DONE ) {
		throw sqlite_exception(sqlite3_errstr(rc));
	}	
	return rc;
}

inline statement::
statement(database_wrapper const& db, std::string const& sql_expr):
	 stmt_ ( new statement_wrapper(db, sql_expr) ) {}

inline statement& statement::
_reset ( void ) {
	sql_try(sqlite3_reset( *stmt_));
	sql_try(sqlite3_clear_bindings( *stmt_));
	return *this;
}

template< typename... Args > statement& statement::
bind ( Args... args ) {
	_reset();
	return _bind ( 1, args... );
}

inline statement& statement::
exec ( void ) {
	sql_try(sqlite3_step( *stmt_));
	return *this;
}
template< typename F >
statement& statement::
forEach ( F const& callback ) {
	while (sql_try(sqlite3_step( *stmt_)) == SQLITE_ROW) _apply ( FFL(callback), 0 );
	return *this;
}

inline statement::
operator sqlite3_stmt* () const {
	return *stmt_;
}

inline statement::
statement ( void ) {}

template < typename T >
statement& statement::
_bind_one ( int iCol, T const& t ) {
	sql_try(sqlite3_bind( *stmt_, iCol, t));
	return *this;
}


inline statement& statement::
_bind ( int iCol ) {
	return *this;
}

template< typename Head, typename... Tail > statement& statement::
_bind ( int iCol, Head head, Tail... tail ) {
	_bind_one ( iCol, head );
	return _bind ( iCol+1, tail... );
}

inline void statement::
_apply(std::function<void()> callback, int i) {
  callback();
}

template<typename Head, typename... Tail> void statement::
_apply(std::function<void(Head, Tail...)> callback, int i) {
	Head head = (sqlite3_column_type( *stmt_, i) == SQLITE_NULL) ?
		  Head() : sqlite3_column<Head>( *stmt_, i);
  std::function<void(Tail...)> partial = [&](Tail&&...tail) { 
  	return std::move(callback)(std::move(head), std::forward<Tail>(tail)...);
  };
  _apply(partial, ++i);
}

inline database::
database ( std::string const & db_name )
	: db_ ( new database_wrapper(db_name) ) {}
 
inline database::
database ( void ) {}

inline void database::
open ( std::string const & db_name ) {
  db_ . reset ();
  db_ . reset ( new database_wrapper(db_name) );
}

inline void database::
close ( void ) {
  db_ . reset ();
}

inline statement database::
prepare ( std::string const& sql_expr ) const {
	return statement( *db_, sql_expr);
}

inline void database::
exec ( std::string const& sql_expr ) const {
	prepare(sql_expr).exec();
}

inline database::
operator sqlite3* () const {
	return  *db_;
}

}
