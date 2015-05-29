/// json.h
/// Shaun Harker
/// 2015-05-26

#ifndef DSGRN_H
#define DSGRN_H

#include <memory>
#include <stack>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <string>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <cctype>

namespace JSON {
class Value;
class Array;
class Object;
class String;
class Integer;

std::shared_ptr<Value> 
parse ( std::string const& str );

std::string 
stringify ( std::shared_ptr<Value> json );

std::shared_ptr<Array> 
toArray ( std::shared_ptr<Value> );

std::shared_ptr<Object> 
toObject ( std::shared_ptr<Value> );

std::shared_ptr<String> 
toString ( std::shared_ptr<Value> );

std::shared_ptr<Integer> 
toInteger ( std::shared_ptr<Value> );

// Simple Value handling class.
// Limitations: Only understands
//  Array
//  Object
//  String
//  only Integer Numbers.

class Value {
public:
  /// stringify
  virtual std::string
  stringify ( void ) const = 0;

  /// Array methods
  virtual std::shared_ptr<Value const> operator [] ( uint64_t i ) const {
    throw std::runtime_error ( "Value: Cannot call accessor on non-Array.");
    return std::shared_ptr<Value const>(this);  
  }
  virtual std::shared_ptr<Value> operator [] ( uint64_t i ) {
    throw std::runtime_error ( "Value: Cannot call accessor on non-Array.");
    return std::shared_ptr<Value>(this);
  }

  /// Object methods
  virtual std::shared_ptr<Value const> operator [] ( std::string const& name ) const {
    throw std::runtime_error ( "Value: Cannot call accessor on non-Object.");
    return std::shared_ptr<Value const>(this);
  }
  virtual std::shared_ptr<Value> operator [] ( std::string const& name ) {
    throw std::runtime_error ( "Value: Cannot call accessor on non-Object.");
    return std::shared_ptr<Value>(this);
  }

  /// Integer methods
  virtual operator int32_t () const {
    throw std::runtime_error ( "Value: Cannot cast non-integer type to integer.");
    return 0;
  }
  virtual operator int64_t () const { 
    throw std::runtime_error ( "Value: Cannot cast non-integer type to integer.");
    return 0;
  }
  virtual operator uint32_t () const { 
    throw std::runtime_error ( "Value: Cannot cast non-integer type to integer.");
    return 0;
  }
  virtual operator uint64_t () const { 
    throw std::runtime_error ( "Value: Cannot cast non-integer type to integer.");
    return 0;
  }

  /// String methods
  virtual operator std::string () const { 
    throw std::runtime_error ( "Value: Cannot cast non-string type to string.");
    return std::string ();
  }

};

class Array : public Value {
public:
  Array ( void ) {}
  Array ( std::vector<std::shared_ptr<Value>> const& data ) : data_(data) {}
  typedef std::vector<std::shared_ptr<Value>>::const_iterator iterator;
  iterator begin ( void ) const { return data_ . begin (); }
  iterator end ( void ) const { return data_ . end (); }
  uint64_t size ( void ) const { return data_ . size (); }
  virtual std::shared_ptr<Value const> operator [] ( uint64_t i ) const {return data_[i];}
  virtual std::shared_ptr<Value> operator [] ( uint64_t i ) {return data_[i];}
  virtual std::string stringify ( void ) const {
    std::stringstream ss;
    ss << "["; 
    bool first = true;
    for(auto ptr : data_ ) { 
      if ( first ) first = false; else ss << ",";
      ss << ptr -> stringify (); 
    }
    ss << "]";
    return ss . str ();
  }

private:
  std::vector<std::shared_ptr<Value>> data_;
};

class Object : public Value {
public:
  Object ( void ) {}
  Object ( std::unordered_map < std::string, std::shared_ptr<Value> > const& data ) : data_(data) {}
  typedef std::unordered_map<std::string, std::shared_ptr<Value>>::const_iterator iterator;
  iterator begin ( void ) const { return data_ . begin (); }
  iterator end ( void ) const { return data_ . end (); }
  uint64_t size ( void ) const { return data_ . size (); }
  virtual std::shared_ptr<Value const> operator [] ( std::string const& name ) const {return data_.find(name)->second;}
  virtual std::shared_ptr<Value> operator [] ( std::string const& name ) {return data_[name];}
  virtual std::string stringify ( void ) const {
    std::stringstream ss;
    ss << "{"; 
    bool first = true;
    for(auto const& pair : data_ ) { 
      if ( first ) first = false; else ss << ",";
      //ss << "$" << pair.first << "#"; 
      ss << "\"" << pair.first << "\""; 
      ss << ":";
      ss << pair.second -> stringify ();
      //ss << "$" << pair.second -> stringify () << "#";
    }
    ss << "}";
    return ss . str ();    
  }
private:
  std::unordered_map < std::string, std::shared_ptr<Value> > data_;
};

class Integer : public Value {
public:
  Integer ( void ) {}
  Integer ( int64_t data ) : data_(data) {}
  virtual operator int32_t () const { return data_; }
  virtual operator int64_t () const { return data_; }
  virtual operator uint32_t () const { return data_; }
  virtual operator uint64_t () const { return data_; }
  virtual std::string stringify ( void ) const {
    return std::to_string ( data_ );
  }
private:
  int64_t data_;
};

class String : public Value {
public:
  String ( void ) {}
  String ( std::string const& data ) : data_(data) {}
  virtual operator std::string () const { return data_; }
  virtual std::string stringify ( void ) const {
    return "\"" + data_ + "\"";
  }
private:
  std::string data_;
};


inline std::shared_ptr<Value> 
parse ( std::string const& str ) {
  // Build balanced parentheses structure
  std::vector<uint64_t> left;
  std::unordered_map<uint64_t, uint64_t> match;
  std::stack<uint64_t> work_stack;
  bool quote_mode = false;
  bool numeric_mode = false;
  uint64_t N = str . size ();
  for ( uint64_t i = 0; i < N; ++ i ) {
    //std::cout << i << " " << str[i] << "\n";
    if ( numeric_mode ) {
      if ( str [ i ] < '0' || str [ i ] > '9' ) {
        numeric_mode = false;
        //std::cout << "NUMERIC MODE OFF\n";
        match [ work_stack . top () ] = i;
        work_stack . pop ();     
        -- i; // redo this character   
      }
    } else if ( quote_mode ) {
      if ( str [ i ] == '\"' ) { // quote ended
        quote_mode = false;
        //std::cout << "QUOTE MODE OFF\n";
        match [ work_stack . top () ] = i;
        work_stack . pop ();
      }
      if ( str [ i ] == '\\' ) ++ i; // skip escaped character
    } else {
      if ( str[i] >= '0' && str[i] <= '9' ) {
        work_stack . push ( i );
        left . push_back ( i );
        numeric_mode = true;
        //std::cout << "NUMERIC MODE ON\n";
      }
      if ( str[i] == '\"' ) { // quote begins
        work_stack . push ( i );
        left . push_back ( i );
        quote_mode = true;
        //std::cout << "QUOTE MODE ON\n";
      }
      if ( str[i] == '{' || str[i] == '[' ) { 
        work_stack . push ( i );
        left . push_back ( i );
        //std::cout << "LEFT\n";
      }
      if ( str[i] == '}' || str[i] == ']' ) { 
        //std::cout << "RIGHT MATCH TO " << work_stack . top () << "\n";
        match [ work_stack . top () ] = i;
        if ( work_stack . empty () ||
             (str[work_stack.top()] == '{' && str[i] != '}') ||
             (str[work_stack.top()] == '[' && str[i] != ']')  ) {
          throw std::runtime_error ("parse: Invalid Value string." );
        }
        work_stack . pop ();
      }
    }
  }
  if ( not work_stack . empty () ) {
    throw std::runtime_error ("parse: Invalid Value string." );
  }

  uint64_t item = 0;
  
  uint64_t num_items = left . size ();
  std::vector<uint64_t> right ( num_items );
  for ( uint64_t i = 0; i < num_items; ++ i ) {
    right [ i ] = match[left[i]];
  }
  left . push_back ( str . size () ); // causes termination

  auto isChild = [&] (uint64_t child, uint64_t parent ) {
    return ( left[child] < right[parent] );
  };

  std::function<std::shared_ptr<Value>(void)> 
    parse_object, parse_array, parse_string, parse_integer, parse_item;
  parse_object = [&](){
    //std::cout << "parse_object begin " << item << "\n";
    uint64_t this_item = item ++;
    std::unordered_map < std::string, std::shared_ptr<Value> > data;
    bool onkey = true;
    std::string key;
    while ( isChild( item, this_item) ) {
      if ( onkey ) {
        key = str . substr (left[item]+1, right[item]-left[item]-1);
        onkey = false;
        //std::cout << "parse_object key (" << item << ") " << key << "\n";
        ++ item;
      } else {
        data [ key ] = parse_item ();
        onkey = true;
      }
    }
    if ( not onkey ) {
      throw std::runtime_error ("parse: Invalid Value string." );
    }
    //std::cout << "parse_object return " << this_item << "\n";
    return std::shared_ptr<Value> ( new Object ( data ) );
  };

  parse_array = [&](){
    //std::cout << "parse_array begin " << item << "\n";
    uint64_t this_item = item ++;
    std::vector<std::shared_ptr<Value>> data;
    while ( isChild ( item, this_item ) ) {
      data . push_back ( parse_item () );
    }
    //std::cout << "parse_array return " << this_item << "\n";
    return std::shared_ptr<Value> ( new Array ( data ) );  
  };

  parse_string = [&](){
    std::string data = str . substr (left[item]+1, right[item]-left[item]-1);
    //std::cout << "parse_string (" << item << "):\"" << data << "\"\n";
    ++ item;
    return std::shared_ptr<Value> ( new String ( data ) );
  };
  parse_integer = [&](){
    std::string data = str . substr (left[item], right[item]-left[item]);
    //std::cout << "parse_integer (" << item << "):\"" << data << "\"\n";
    ++ item;
    return std::shared_ptr<Value> ( new Integer (std::stoll(data)));
  }; 

  parse_item = [&](){
    std::shared_ptr<Value> result;
    switch ( str[left[item]] ) {
      case '{' :
        result = parse_object ();
        break;
      case '[' :
        result = parse_array ();
        break;
      case '\"' :
        result = parse_string ();
        break;
      default :
        result = parse_integer ();
        break;
    }
    return result;
  };

  return parse_item ();
}

inline std::string 
stringify ( std::shared_ptr<Value> json ) {
  return json -> stringify ();
}

inline std::shared_ptr<Array> 
toArray ( std::shared_ptr<Value> ptr ) {
  return std::dynamic_pointer_cast<Array> (ptr);
}

inline std::shared_ptr<Object> 
toObject ( std::shared_ptr<Value> ptr ) {
  return std::dynamic_pointer_cast<Object> (ptr);
}

inline std::shared_ptr<String> 
toString ( std::shared_ptr<Value> ptr ) {
  return std::dynamic_pointer_cast<String> (ptr);

}

inline std::shared_ptr<Integer> 
toInteger ( std::shared_ptr<Value> ptr ) {
  return std::dynamic_pointer_cast<Integer> (ptr);
}

}

#endif
