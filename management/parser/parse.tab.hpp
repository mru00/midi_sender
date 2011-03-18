/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     SIGNAL = 258,
     CONTROLLER = 259,
     NUMBER = 260,
     STRING = 261,
     ID = 262,
     CONTROL = 263,
     NOTEON = 264,
     NOTEOFF = 265,
     AFTERTOUCH = 266,
     PROGRAMCHANGE = 267,
     CHANNELPRESSURE = 268,
     PITCHWHEEL = 269,
     ANALOG = 270,
     DIGITAL = 271,
     STATUS = 272
   };
#endif
/* Tokens.  */
#define SIGNAL 258
#define CONTROLLER 259
#define NUMBER 260
#define STRING 261
#define ID 262
#define CONTROL 263
#define NOTEON 264
#define NOTEOFF 265
#define AFTERTOUCH 266
#define PROGRAMCHANGE 267
#define CHANNELPRESSURE 268
#define PITCHWHEEL 269
#define ANALOG 270
#define DIGITAL 271
#define STATUS 272




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 41 "parse.ypp"
{
	int ival;
	Signal* sig;
	char* str;
}
/* Line 1489 of yacc.c.  */
#line 89 "parse.tab.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

