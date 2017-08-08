/*-----------------------------------------------------------------------------
 * Copyright (c) 2016, 2017 Oracle and/or its affiliates.  All rights reserved.
 * This program is free software: you can modify it and/or redistribute it
 * under the terms of:
 *
 * (i)  the Universal Permissive License v 1.0 or at your option, any
 *      later version (http://oss.oracle.com/licenses/upl); and/or
 *
 * (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * SetupTest.sql
 *   Creates a test user and populates its schema with the tables and
 *   packages necessary for performing the test suite. It also creates
 *   a proxy user for testing proxying.
 *
 * Run this like:
 *   sqlplus / as sysdba @SetupTest <odpicuser> <password> <dirname> <dirpath>
 *
 * where the parameters are the names you choose to run the tests.
 * The <dirpath> value should be a valid OS directory that the
 * database server can write to.  This is used in TestBFILE.c
 *---------------------------------------------------------------------------*/

set echo off termout on feedback off verify off

-- Set default schema values if not passed in as parameters
column 1 new_value 1 noprint
column 2 new_value 2 noprint
column 3 new_value 3 noprint
column 4 new_value 4 noprint
select '' "1", '' "2", '' "3", '' "4" from dual where 1 = 0;
define username = &1 "ODPIC"
define password = &2 "welcome"
define dirname = &3 "ODPIC_DIR"
define dirpath = &4 "/tmp"

-- Convert names to uppercase
col username new_value username noprint
col dirname new_value dirname noprint
select upper('&username') username, upper('&dirname') dirname from dual;

-- Set Proxy username
define usernameprx = &username._PROXY

prompt Creating users &username and &usernameprx
prompt Creating directory &dirname for &dirpath
prompt

set echo on verify on feedback on

-- Drop existing users, if present
begin
  for r in
      ( select username
        from dba_users
        where username in ('&username', '&usernameprx')
      ) loop
    execute immediate 'drop user ' || r.username || ' cascade';
  end loop;
end;
/

-- Drop directory
begin
  for r in
      ( select directory_name
        from dba_directories
        where directory_name = '&dirname'
      ) loop
    execute immediate 'drop directory ' || r.directory_name;
  end loop;
end;
/

CREATE DIRECTORY &dirname AS '&dirpath';

-- verify directory exists
declare
    t_Temp pls_integer;
begin
    select count(*)
    into t_Temp
    from dba_directories
    where directory_name = upper('&dirname');

    if t_Temp = 0 then
        raise_application_error(-20000,
                'Please pass a valid directory that the DB server can write to');
    end if;
end;
/

whenever sqlerror exit failure

alter session set nls_date_format = 'YYYY-MM-DD HH24:MI:SS';
alter session set nls_numeric_characters='.,';

create user &username identified by &password
quota unlimited on users
default tablespace users;

create user &usernameprx identified by &password;
alter user &usernameprx grant connect through &username;

grant create session to &usernameprx;

grant
  create session,
  create table,
  create procedure,
  create type,
  change notification
to &username;

grant read on directory &dirname to &username;

grant select on v_$session to &username;

-- create types
create type &username..udt_SubObject as object (
  SubNumberValue        number,
  SubStringValue        varchar2(60)
);
/

create type &username..udt_ObjectArray as varray(10) of &username..udt_SubObject;
/

create type &username..udt_Object as object (
  NumberValue           number,
  StringValue           varchar2(60),
  FixedCharValue        char(10),
  DateValue             date,
  TimestampValue        timestamp,
  SubObjectValue        &username..udt_SubObject,
  SubObjectArray        &username..udt_ObjectArray
);
/

create type &username..udt_Array as varray(10) of number;
/

create type &username..udt_ObjectDataTypes as object (
  StringCol         varchar2(60),
  UnicodeCol        nvarchar2(60),
  FixedCharCol      char(30),
  FixedUnicodeCol   nchar(30),
  IntCol            number,
  NumberCol         number(9,2),
  DateCol           date,
  TimestampCol      timestamp,
  TimestampTZCol    timestamp with time zone,
  TimestampLTZCol   timestamp with local time zone,
  BinaryFltCol      binary_float,
  BinaryDoubleCol   binary_double
);
/

create type &username..udt_ObjectDataTypesArray as varray(10) of
            &username..udt_ObjectDataTypes;
/

create type &username..udt_NestedArray is table of &username..udt_SubObject;
/

-- create tables
create table &username..TestNumbers (
  IntCol                number(9) not null,
  NumberCol             number(9, 2) not null,
  FloatCol              float not null,
  UnconstrainedCol      number not null,
  NullableCol           number(38)
);

create table &username..TestStrings (
  IntCol                number(9) not null,
  StringCol             varchar2(20) not null,
  RawCol                raw(30) not null,
  FixedCharCol          char(40) not null,
  NullableCol           varchar2(50)
);

create table &username..TestUnicodes (
  IntCol                number(9) not null,
  UnicodeCol            nvarchar2(20) not null,
  FixedUnicodeCol       nchar(40) not null,
  NullableCol           nvarchar2(50)
);

create table &username..TestDates (
  IntCol                number(9) not null,
  DateCol               date not null,
  NullableCol           date
);

create table &username..TestCLOBs (
  IntCol                number(9) not null,
  CLOBCol               clob not null
);

create table &username..TestNCLOBs (
  IntCol                number(9) not null,
  NCLOBCol              nclob not null
);

create table &username..TestBLOBs (
  IntCol                number(9) not null,
  BLOBCol               blob not null
);

create table &username..TestBFILEs (
  IntCol                number(9) not null,
  BFILECol              bfile not null
);

create table &username..TestLongs (
  IntCol                number(9) not null,
  LongCol               long not null
);

create table &username..TestLongsAlter (
  IntCol                number(9),
  LongCol               long
);

create table &username..TestLongRaws (
  IntCol                number(9) not null,
  LongRawCol            long raw not null
);

create table &username..TestTempTable (
  IntCol                number(9) not null,
  StringCol             varchar2(100),
  constraint TestTempTable_pk primary key (IntCol)
);

create table &username..TestArrayDML (
  IntCol                number(9) not null,
  StringCol             varchar2(100),
  IntCol2               number(3),
  constraint TestArrayDML_pk primary key (IntCol)
);

create table &username..TestObjects (
  IntCol                number(9) not null,
  ObjectCol             &username..udt_Object,
  ArrayCol              &username..udt_Array
);

create table &username..TestTimestamps (
  IntCol                number(9) not null,
  TimestampCol          timestamp not null,
  TimestampTZCol        timestamp with time zone not null,
  TimestampLTZCol       timestamp with local time zone not null,
  NullableCol           timestamp
);

create table &username..TestIntervals (
  IntCol                number(9) not null,
  IntervalCol           interval day to second not null,
  NullableCol           interval day to second
);

create table &username..TestObjectDataTypes (
  ObjectCol             &username..udt_ObjectDataTypes
);

create table &username..TestObjectDataTypesVarray (
  ObjectCol             &username..udt_ObjectDataTypesArray
);

create table &username..TestDataTypes (
  StringCol             varchar2(100) not null,
  UnicodeCol            nvarchar2(100) not null,
  FixedCharCol          char(100) not null,
  FixedUnicodeCol       nchar(100) not null,
  RawCol                raw(30) not null,
  FloatCol              float not null,
  DoublePrecCol         double precision not null,
  IntCol                number(9) not null,
  NumberCol             number(9,2) not null,
  DateCol               date not null,
  TimestampCol          timestamp not null,
  TimestampTZCol        timestamp with time zone not null,
  TimestampLTZCol       timestamp with local time zone not null,
  IntervalDSCol         interval day to second not null,
  IntervalYMCol         interval year to month not null,
  BinaryFltCol          binary_float not null,
  BinaryDoubleCol       binary_double not null,
  CLOBCol               clob,
  NCLOBCol              nclob,
  BLOBCol               blob,
  BFILECol              bfile,
  LongCol               long not null,
  UnconstrainedCol      number not null,
  constraint TestDataTypes_pk primary key (IntCol)
);

create table &username..TestOrgIndex (  
    IntCol              number(9) not null,
    StringCol           varchar2(30) not null,
    constraint pk_index primary key (IntCol)
) organization index;

-- populate tables
begin
  for i in 1..30 loop
    insert into &username..TestOrgIndex
    values (i, 'String value ' || i);
  end loop;
end;
/

begin
  for i in 1..10 loop
    insert into &username..TestNumbers
    values (i, i + i * 0.25, i + i * .75, i * i * i + i *.5,
        decode(mod(i, 2), 0, null, power(143, i)));
  end loop;
end;
/

declare

  t_RawValue            raw(30);

  function ConvertHexDigit(a_Value number) return varchar2 is
  begin
    if a_Value between 0 and 9 then
      return to_char(a_Value);
    end if;
    return chr(ascii('A') + a_Value - 10);
  end;

  function ConvertToHex(a_Value varchar2) return varchar2 is
    t_HexValue          varchar2(60);
    t_Digit             number;
  begin
    for i in 1..length(a_Value) loop
      t_Digit := ascii(substr(a_Value, i, 1));
      t_HexValue := t_HexValue ||
          ConvertHexDigit(trunc(t_Digit / 16)) ||
          ConvertHexDigit(mod(t_Digit, 16));
    end loop;
    return t_HexValue;
  end;

begin
  for i in 1..10 loop
    t_RawValue := hextoraw(ConvertToHex('Raw ' || to_char(i)));
    insert into &username..TestStrings
    values (i, 'String ' || to_char(i), t_RawValue,
        'Fixed Char ' || to_char(i),
        decode(mod(i, 2), 0, null, 'Nullable ' || to_char(i)));
  end loop;
end;
/

begin
  for i in 1..10 loop
    insert into &username..TestUnicodes
    values (i, 'Unicode ' || unistr('\3042') || ' ' || to_char(i),
        'Fixed Unicode ' || to_char(i),
        decode(mod(i, 2), 0, null, unistr('Nullable ') || to_char(i)));
  end loop;
end;
/

begin
  for i in 1..10 loop
    insert into &username..TestDates
    values (i, to_date(20021209, 'YYYYMMDD') + i + i * .1,
        decode(mod(i, 2), 0, null,
        to_date(20021209, 'YYYYMMDD') + i + i + i * .15));
  end loop;
end;
/

begin
  for i in 1..10 loop
    insert into &username..TestTimestamps
    values (i, to_timestamp('20021209', 'YYYYMMDD') +
            to_dsinterval(to_char(i) || ' 00:00:' || to_char(i * 2) || '.' ||
                    to_char(i * 50)),
        to_timestamp_tz('20021210 00:00:00 ' || to_char(i - 5, 'S00') ||
                ':00', 'YYYYMMDD HH24:MI:SS TZH:TZM') +
            to_dsinterval(to_char(i) || ' 00:00:' || to_char(i * 3) || '.' ||
                    to_char(i * 75)),
        to_timestamp_tz('20021211 00:00:00 ' || to_char(i - 8, 'S00') ||
                ':00', 'YYYYMMDD HH24:MI:SS TZH:TZM') +
            to_dsinterval(to_char(i) || ' 00:00:' || to_char(i * 4) || '.' ||
                    to_char(i * 100)),
        decode(mod(i, 2), 0, to_timestamp(null, 'YYYYMMDD'),
        to_timestamp('20021209', 'YYYYMMDD') +
            to_dsinterval(to_char(i + 1) || ' 00:00:' ||
                    to_char(i * 3) || '.' || to_char(i * 125))));
  end loop;
end;
/

begin
  for i in 1..10 loop
    insert into &username..TestIntervals
    values (i, to_dsinterval(to_char(i) || ' ' || to_char(i) || ':' ||
            to_char(i * 2) || ':' || to_char(i * 3)),
            decode(mod(i, 2), 0, to_dsinterval(null),
            to_dsinterval(to_char(i + 5) || ' ' || to_char(i + 2) || ':' ||
            to_char(i * 2 + 5) || ':' || to_char(i * 3 + 5))));
  end loop;
end;
/

insert into &username..TestObjects values (1,
    &username..udt_Object(1, 'First row', 'First',
        to_date(20070306, 'YYYYMMDD'),
        to_timestamp('20080912 16:40:00', 'YYYYMMDD HH24:MI:SS'),
        &username..udt_SubObject(11, 'Sub object 1'),
        &username..udt_ObjectArray(
                &username..udt_SubObject(5, 'first element'),
                &username..udt_SubObject(6, 'second element'))),
    &username..udt_Array(5, 10, null, 20));

insert into &username..TestObjects values (2, null,
    &username..udt_Array(3, null, 9, 12, 15));

insert into &username..TestObjects values (3,
    &username..udt_Object(3, 'Third row', 'Third',
        to_date(20070621, 'YYYYMMDD'),
        to_timestamp('20071213 07:30:45', 'YYYYMMDD HH24:MI:SS'),
        &username..udt_SubObject(13, 'Sub object 3'),
        &username..udt_ObjectArray(
                &username..udt_SubObject(10, 'element #1'),
                &username..udt_SubObject(20, 'element #2'),
                &username..udt_SubObject(30, 'element #3'),
                &username..udt_SubObject(40, 'element #4'))), null);

commit;

-- create procedures for testing callproc()
create procedure &username..proc_Test (
  a_InValue             varchar2,
  a_InOutValue          in out number,
  a_OutValue            out number
) as
begin
  a_InOutValue := a_InOutValue * length(a_InValue);
  a_OutValue := length(a_InValue);
end;
/

create procedure &username..proc_TestNoArgs as
begin
  null;
end;
/

-- create functions for testing callfunc()
create function &username..func_Test (
  a_String              varchar2,
  a_ExtraAmount         number
) return number as
begin
  return length(a_String) + a_ExtraAmount;
end;
/

create function &username..func_TestNoArgs
return number as
begin
  return 712;
end;
/

-- create packages
create or replace package &username..pkg_TestStringArrays as

  type udt_StringList is table of varchar2(100) index by binary_integer;

  function TestInArrays (
    a_StartingLength    number,
    a_Array             udt_StringList
  ) return number;

  procedure TestInOutArrays (
    a_NumElems          number,
    a_Array             in out nocopy udt_StringList
  );

  procedure TestOutArrays (
    a_NumElems          number,
    a_Array             out nocopy udt_StringList
  );

  procedure TestIndexBy (
    a_Array             out nocopy udt_StringList
  );

end;
/

create or replace package body &username..pkg_TestStringArrays as

  function TestInArrays (
    a_StartingLength    number,
    a_Array             udt_StringList
  ) return number is
    t_Length            number;
  begin
    t_Length := a_StartingLength;
    for i in 1..a_Array.count loop
      t_Length := t_Length + length(a_Array(i));
    end loop;
    return t_Length;
  end;

  procedure TestInOutArrays (
    a_NumElems          number,
    a_Array             in out udt_StringList
  ) is
  begin
    for i in 1..a_NumElems loop
      a_Array(i) := 'Converted element # ' ||
          to_char(i) || ' originally had length ' ||
          to_char(length(a_Array(i)));
    end loop;
  end;

  procedure TestOutArrays (
    a_NumElems          number,
    a_Array             out udt_StringList
  ) is
  begin
    for i in 1..a_NumElems loop
      a_Array(i) := 'Test out element # ' || to_char(i);
    end loop;
  end;

  procedure TestIndexBy (
    a_Array             out nocopy udt_StringList
  ) is
  begin
    a_Array(-1048576) := 'First element';
    a_Array(-576) := 'Second element';
    a_Array(284) := 'Third element';
    a_Array(8388608) := 'Fourth element';
  end;

end;
/

create or replace package &username..pkg_TestUnicodeArrays as

  type udt_UnicodeList is table of nvarchar2(100) index by binary_integer;

  function TestInArrays (
    a_StartingLength    number,
    a_Array             udt_UnicodeList
  ) return number;

  procedure TestInOutArrays (
    a_NumElems          number,
    a_Array             in out nocopy udt_UnicodeList
  );

  procedure TestOutArrays (
    a_NumElems          number,
    a_Array             out nocopy udt_UnicodeList
  );

end;
/

create or replace package body &username..pkg_TestUnicodeArrays as

  function TestInArrays (
    a_StartingLength    number,
    a_Array             udt_UnicodeList
  ) return number is
    t_Length            number;
  begin
    t_Length := a_StartingLength;
    for i in 1..a_Array.count loop
      t_Length := t_Length + length(a_Array(i));
    end loop;
    return t_Length;
  end;

  procedure TestInOutArrays (
    a_NumElems          number,
    a_Array             in out udt_UnicodeList
  ) is
  begin
    for i in 1..a_NumElems loop
      a_Array(i) := unistr('Converted element ' || unistr('\3042') ||
          ' # ') || to_char(i) || ' originally had length ' ||
          to_char(length(a_Array(i)));
    end loop;
  end;

  procedure TestOutArrays (
    a_NumElems          number,
    a_Array             out udt_UnicodeList
  ) is
  begin
    for i in 1..a_NumElems loop
      a_Array(i) := unistr('Test out element ') || unistr('\3042') || ' # ' ||
          to_char(i);
    end loop;
  end;

end;
/

create or replace package &username..pkg_TestNumberArrays as

  type udt_NumberList is table of number index by binary_integer;

  function TestInArrays (
    a_StartingValue     number,
    a_Array             udt_NumberList
  ) return number;

  procedure TestInOutArrays (
    a_NumElems          number,
    a_Array             in out nocopy udt_NumberList
  );

  procedure TestOutArrays (
    a_NumElems          number,
    a_Array             out nocopy udt_NumberList
  );

end;
/

create or replace package body &username..pkg_TestNumberArrays as

  function TestInArrays (
    a_StartingValue     number,
    a_Array             udt_NumberList
  ) return number is
    t_Value             number;
  begin
    t_Value := a_StartingValue;
    for i in 1..a_Array.count loop
      t_Value := t_Value + a_Array(i);
    end loop;
    return t_Value;
  end;

  procedure TestInOutArrays (
    a_NumElems          number,
    a_Array             in out udt_NumberList
  ) is
  begin
    for i in 1..a_NumElems loop
      a_Array(i) := a_Array(i) * 10;
    end loop;
  end;

  procedure TestOutArrays (
    a_NumElems          number,
    a_Array             out udt_NumberList
  ) is
  begin
    for i in 1..a_NumElems loop
      a_Array(i) := i * 100;
    end loop;
  end;

end;
/

create or replace package &username..pkg_TestDateArrays as

  type udt_DateList is table of date index by binary_integer;

  function TestInArrays (
    a_StartingValue     number,
    a_BaseDate          date,
    a_Array             udt_DateList
  ) return number;

  procedure TestInOutArrays (
    a_NumElems          number,
    a_Array             in out nocopy udt_DateList
  );

  procedure TestOutArrays (
    a_NumElems          number,
    a_Array             out nocopy udt_DateList
  );

end;
/

create or replace package body &username..pkg_TestDateArrays as

  function TestInArrays (
    a_StartingValue     number,
    a_BaseDate          date,
    a_Array             udt_DateList
  ) return number is
    t_Value             number;
  begin
    t_Value := a_StartingValue;
    for i in 1..a_Array.count loop
      t_Value := t_Value + a_Array(i) - a_BaseDate;
    end loop;
    return t_Value;
  end;

  procedure TestInOutArrays (
    a_NumElems          number,
    a_Array             in out udt_DateList
  ) is
  begin
    for i in 1..a_NumElems loop
      a_Array(i) := a_Array(i) + 7;
    end loop;
  end;

  procedure TestOutArrays (
    a_NumElems          number,
    a_Array             out udt_DateList
  ) is
  begin
    for i in 1..a_NumElems loop
      a_Array(i) := to_date(20021212, 'YYYYMMDD') + i * 1.2;
    end loop;
  end;

end;
/

create or replace package &username..pkg_TestOutCursors as

  type udt_RefCursor is ref cursor;

  procedure TestOutCursor (
    a_MaxIntValue       number,
    a_Cursor            out udt_RefCursor
  );

end;
/

create or replace package body &username..pkg_TestOutCursors as

  procedure TestOutCursor (
    a_MaxIntValue       number,
    a_Cursor            out udt_RefCursor
  ) is
  begin
    open a_Cursor for
      select
        IntCol,
        StringCol
      from TestStrings
      where IntCol <= a_MaxIntValue
      order by IntCol;
  end;

end;
/

create or replace package &username..pkg_TestBooleans as

    type udt_BooleanList is table of boolean index by binary_integer;

    function GetStringRep (
        a_Value             boolean
    ) return varchar2;

    function IsLessThan10 (
        a_Value             number
    ) return boolean;

    function TestInArrays (
        a_Value             udt_BooleanList
    ) return number;

    procedure TestOutArrays (
        a_NumElements       number,
        a_Value             out nocopy udt_BooleanList
    );

end;
/

create or replace package body &username..pkg_TestBooleans as

    function GetStringRep (
        a_Value             boolean
    ) return varchar2 is
    begin
        if a_Value is null then
            return 'NULL';
        elsif a_Value then
            return 'TRUE';
        end if;
        return 'FALSE';
    end;

    function IsLessThan10 (
        a_Value             number
    ) return boolean is
    begin
        return a_Value < 10;
    end;

    function TestInArrays (
        a_Value             udt_BooleanList
    ) return number is
        t_Result            pls_integer;
    begin
        t_Result := 0;
        for i in 1..a_Value.count loop
            if a_Value(i) then
                t_Result := t_Result + 1;
            end if;
        end loop;
        return t_Result;
    end;

    procedure TestOutArrays (
        a_NumElements       number,
        a_Value             out nocopy udt_BooleanList
    ) is
    begin
        for i in 1..a_NumElements loop
            a_Value(i) := (mod(i, 2) = 1);
        end loop;
    end;

end;
/

create or replace package &username..pkg_TestBindObject as

    function GetStringRep (
        a_Object        udt_Object
    ) return varchar2;

end;
/

create or replace package body &username..pkg_TestBindObject as

    function GetStringRep (
        a_Object        udt_SubObject
    ) return varchar2 is
    begin
        if a_Object is null then
            return 'null';
        end if;
        return 'udt_SubObject(' ||
                nvl(to_char(a_Object.SubNumberValue), 'null') || ', ' ||
                case when a_Object.SubStringValue is null then 'null'
                else '''' || a_Object.SubStringValue || '''' end || ')';
    end;

    function GetStringRep (
        a_Array         udt_ObjectArray
    ) return varchar2 is
        t_StringRep     varchar2(4000);
    begin
        if a_Array is null then
            return 'null';
        end if;
        t_StringRep := 'udt_ObjectArray(';
        for i in 1..a_Array.count loop
            if i > 1 then
                t_StringRep := t_StringRep || ', ';
            end if;
            t_StringRep := t_StringRep || GetStringRep(a_Array(i));
        end loop;
        return t_StringRep || ')';
    end;

    function GetStringRep (
        a_Object        udt_Object
    ) return varchar2 is
    begin
        if a_Object is null then
            return 'null';
        end if;
        return 'udt_Object(' ||
                nvl(to_char(a_Object.NumberValue), 'null') || ', ' ||
                case when a_Object.StringValue is null then 'null'
                else '''' || a_Object.StringValue || '''' end || ', ' ||
                case when a_Object.FixedCharValue is null then 'null'
                else '''' || a_Object.FixedCharValue || '''' end || ', ' ||
                case when a_Object.DateValue is null then 'null'
                else 'to_date(''' ||
                        to_char(a_Object.DateValue, 'YYYY-MM-DD') ||
                        ''', ''YYYY-MM-DD'')' end || ', ' ||
                case when a_Object.TimestampValue is null then 'null'
                else 'to_timestamp(''' || to_char(a_Object.TimestampValue,
                        'YYYY-MM-DD HH24:MI:SS') ||
                        ''', ''YYYY-MM-DD HH24:MI:SS'')' end || ', ' ||
                GetStringRep(a_Object.SubObjectValue) || ', ' ||
                GetStringRep(a_Object.SubObjectArray) || ')';
    end;

end;
/

create or replace package &username..pkg_TestRecords as

    type udt_Record is record (
        NumberValue         number,
        StringValue         varchar2(30),
        DateValue           date,
        TimestampValue      timestamp,
        BooleanValue        boolean
    );

    function GetStringRep (
        a_Value             udt_Record
    ) return varchar2;

    procedure TestOut (
        a_Value             out nocopy udt_Record
    );

end;
/

create or replace package body &username..pkg_TestRecords as

    function GetStringRep (
        a_Value             udt_Record
    ) return varchar2 is
    begin
        return 'udt_Record(' ||
                nvl(to_char(a_Value.NumberValue), 'null') || ', ' ||
                case when a_Value.StringValue is null then 'null'
                else '''' || a_Value.StringValue || '''' end || ', ' ||
                case when a_Value.DateValue is null then 'null'
                else 'to_date(''' ||
                        to_char(a_Value.DateValue, 'YYYY-MM-DD') ||
                        ''', ''YYYY-MM-DD'')' end || ', ' ||
                case when a_Value.TimestampValue is null then 'null'
                else 'to_timestamp(''' || to_char(a_Value.TimestampValue,
                        'YYYY-MM-DD HH24:MI:SS') ||
                        ''', ''YYYY-MM-DD HH24:MI:SS'')' end || ', ' ||
                case when a_Value.BooleanValue is null then 'null'
                when a_Value.BooleanValue then 'true'
                else 'false' end || ')';
    end;

    procedure TestOut (
        a_Value             out nocopy udt_Record
    ) is
    begin
        a_Value.NumberValue := 25;
        a_Value.StringValue := 'String in record';
        a_Value.DateValue := to_date(20160216, 'YYYYMMDD');
        a_Value.TimestampValue := to_timestamp('20160216 18:23:55',
                'YYYYMMDD HH24:MI:SS');
        a_Value.BooleanValue := true;
    end;

end;
/

create or replace package &username..pkg_TestLOBs as

    procedure TestInOutTempClob (
        a_IntValue          number,
        a_CLOB              in out clob
    );

end;
/

create or replace package body &username..pkg_TestLOBs as

    procedure TestInOutTempClob (
        a_IntValue          number,
        a_CLOB              in out clob
    ) is
    begin

        delete from TestClobs
        where IntCol = a_IntValue;

        insert into TestClobs (
            IntCol,
            ClobCol
        ) values (
            a_IntValue,
            a_CLOB
        );

        select ClobCol
        into a_CLOB
        from TestClobs
        where IntCol = a_IntValue;

    end;

end;
/

create or replace procedure &username..proc_TestInOut (
  a_StringCol         in out varchar2,
  a_UnicodeCol        in out nvarchar2,
  a_FloatCol          in out float,
  a_DoublePrecCol     in out double precision,
  a_NumberCol         in out number,
  a_DateCol           in out date,
  a_TimestampCol      in out timestamp,
  a_TimestampTZCol    in out timestamp with time zone,
  a_IntervalDSCol     in out interval day to second,
  a_IntervalYMCol     in out interval year to month,
  a_BinaryFltCol      in out binary_float,
  a_BinaryDoubleCol   in out binary_double
) as
begin
  a_StringCol := 'teststring';
  a_UnicodeCol := 'testunicode';
  a_FloatCol         := a_FloatCol        +  a_FloatCol;
  a_DoublePrecCol    := a_DoublePrecCol   +  a_DoublePrecCol;
  a_NumberCol        := a_NumberCol       +  a_NumberCol;
  a_DateCol          := a_DateCol + interval '1' year;
  a_TimestampCol     := a_TimestampCol + interval '30' minute;
  a_TimestampTZCol   := a_TimestampTZCol + interval '30' minute;
  a_IntervalDSCol    := a_IntervalDSCol + a_IntervalDSCol;
  a_IntervalYMCol    := a_IntervalYMCol + a_IntervalYMCol;
  a_BinaryFltCol     := a_BinaryFltCol    +  a_BinaryFltCol;
  a_BinaryDoubleCol  := a_BinaryDoubleCol +  a_BinaryDoubleCol;
end;
/

-- create type and table for testing advanced queuing
create or replace type &username..udt_Book as object (
    Title varchar2(100),
    Authors varchar2(100),
    Price number(5,2)
);
/

exec dbms_aqadm.create_queue_table('&username..BOOK_QUEUE', '&username..UDT_BOOK');
exec dbms_aqadm.create_queue('&username..BOOKS', '&username..BOOK_QUEUE');
exec dbms_aqadm.start_queue('&username..BOOKS');

exit
