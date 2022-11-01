/*-----------------------------------------------------------------------------
 * Copyright (c) 2016, 2022, Oracle and/or its affiliates.
 *
 * This software is dual-licensed to you under the Universal Permissive License
 * (UPL) 1.0 as shown at https://oss.oracle.com/licenses/upl and Apache License
 * 2.0 as shown at http://www.apache.org/licenses/LICENSE-2.0. You may choose
 * either license.*
 *
 * If you elect to accept the software under the Apache License, Version 2.0,
 * the following applies:
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * SetupSamples.sql
 *   Creates schemas and populates them with the tables and packages necessary
 * for running the various included samples.
 *
 * Run this like:
 *   sqlplus / as sysdba @SetupSamples
 *
 * Note that the script SampleEnv.sql should be modified if you would like to
 * use something other than the default configuration.
 *---------------------------------------------------------------------------*/

whenever sqlerror exit failure

-- drop existing users, if present
@@DropSamples.sql

-- create directory
CREATE DIRECTORY &dir_name AS '&dir_path';

alter session set nls_date_format = 'YYYY-MM-DD HH24:MI:SS';
alter session set nls_numeric_characters='.,';

create user &main_user identified by "&main_password"
quota unlimited on users
default tablespace users;

create user &proxy_user identified by "&proxy_password";
alter user &proxy_user grant connect through &main_user;

grant create session to &proxy_user;

grant
    create session,
    create table,
    create procedure,
    create type,
    change notification
to &main_user;

grant read on directory &dir_name to &main_user;

grant select on v_$session to &main_user;

begin

    for r in
            ( select role
              from dba_roles
              where role in ('SODA_APP')
            ) loop
        execute immediate 'grant ' || r.role || ' to &main_user';
    end loop;

end;
/

-- create types
create type &main_user..udt_SubObject as object (
    SubNumberValue                      number,
    SubStringValue                      varchar2(60)
);
/

create type &main_user..udt_ObjectArray as
    varray(10) of &main_user..udt_SubObject;
/

create type &main_user..udt_Object as object (
    NumberValue                         number,
    StringValue                         varchar2(60),
    FixedCharValue                      char(10),
    DateValue                           date,
    TimestampValue                      timestamp,
    SubObjectValue                      &main_user..udt_SubObject,
    SubObjectArray                      &main_user..udt_ObjectArray
);
/

create type &main_user..udt_Array as varray(10) of number;
/

create type &main_user..udt_ObjectDataTypes as object (
    StringCol                           varchar2(60),
    UnicodeCol                          nvarchar2(60),
    FixedCharCol                        char(30),
    FixedUnicodeCol                     nchar(30),
    IntCol                              number,
    NumberCol                           number(9,2),
    DateCol                             date,
    TimestampCol                        timestamp,
    TimestampTZCol                      timestamp with time zone,
    TimestampLTZCol                     timestamp with local time zone,
    BinaryFltCol                        binary_float,
    BinaryDoubleCol                     binary_double
);
/

create type &main_user..udt_ObjectDataTypesArray as
        varray(10) of &main_user..udt_ObjectDataTypes;
/

create type &main_user..udt_NestedArray is table of &main_user..udt_SubObject;
/

-- create tables
create table &main_user..DemoNumbers (
    IntCol                              number(9) not null,
    NumberCol                           number(9, 2) not null,
    FloatCol                            float not null,
    UnconstrainedCol                    number not null,
    NullableCol                         number(38)
);

create table &main_user..DemoStrings (
    IntCol                              number(9) not null,
    StringCol                           varchar2(20) not null,
    RawCol                              raw(30) not null,
    FixedCharCol                        char(40) not null,
    NullableCol                         varchar2(50)
);

create table &main_user..DemoUnicodes (
    IntCol                              number(9) not null,
    UnicodeCol                          nvarchar2(20) not null,
    FixedUnicodeCol                     nchar(40) not null,
    NullableCol                         nvarchar2(50)
);

create table &main_user..DemoDates (
    IntCol                              number(9) not null,
    DateCol                             date not null,
    NullableCol                         date
);

create table &main_user..DemoCLOBs (
    IntCol                              number(9) not null,
    CLOBCol                             clob not null
);

create table &main_user..DemoNCLOBs (
    IntCol                              number(9) not null,
    NCLOBCol                            nclob not null
);

create table &main_user..DemoBLOBs (
    IntCol                              number(9) not null,
    BLOBCol                             blob not null
);

create table &main_user..DemoBFILEs (
    IntCol                              number(9) not null,
    BFILECol                            bfile not null
);

create table &main_user..DemoLongs (
    IntCol                              number(9) not null,
    LongCol                             long not null
);

create table &main_user..DemoLongsAlter (
    IntCol                              number(9),
    LongCol                             long
);

create table &main_user..DemoLongRaws (
    IntCol                              number(9) not null,
    LongRawCol                          long raw not null
);

create table &main_user..DemoTempTable (
    IntCol                              number(9) not null,
    StringCol                           varchar2(100),
    constraint DemoTempTable_pk primary key (IntCol)
);

create table &main_user..DemoArrayDML (
    IntCol                              number(9) not null,
    StringCol                           varchar2(100),
    IntCol2                             number(3),
    constraint DemoArrayDML_pk primary key (IntCol)
);

create table &main_user..DemoObjects (
    IntCol                              number(9) not null,
    ObjectCol                           &main_user..udt_Object,
    ArrayCol                            &main_user..udt_Array
);

create table &main_user..DemoTimestamps (
    IntCol                              number(9) not null,
    TimestampCol                        timestamp not null,
    TimestampTZCol                      timestamp with time zone not null,
    TimestampLTZCol                     timestamp with local time zone not null,
    NullableCol                         timestamp
);

create table &main_user..DemoIntervals (
    IntCol                              number(9) not null,
    IntervalCol                         interval day to second not null,
    NullableCol                         interval day to second
);

create table &main_user..DemoObjectDataTypes (
    ObjectCol                           &main_user..udt_ObjectDataTypes
);

create table &main_user..DemoObjectDataTypesVarray (
    ObjectCol                           &main_user..udt_ObjectDataTypesArray
);

-- populate tables
begin
    for i in 1..10 loop
        insert into &main_user..DemoNumbers
        values (i, i + i * 0.25, i + i * .75, i * i * i + i *.5,
                decode(mod(i, 2), 0, null, power(143, i)));
    end loop;
end;
/

declare

    t_RawValue                          raw(30);

    function ConvertHexDigit(a_Value number) return varchar2 is
    begin
        if a_Value between 0 and 9 then
            return to_char(a_Value);
        end if;
        return chr(ascii('A') + a_Value - 10);
    end;

    function ConvertToHex(a_Value varchar2) return varchar2 is
        t_HexValue                      varchar2(60);
        t_Digit                         number;
    begin
        for i in 1..length(a_Value) loop
            t_Digit := ascii(substr(a_Value, i, 1));
            t_HexValue := t_HexValue || ConvertHexDigit(trunc(t_Digit / 16)) ||
                    ConvertHexDigit(mod(t_Digit, 16));
        end loop;
        return t_HexValue;
    end;

begin
    for i in 1..10 loop
        t_RawValue := hextoraw(ConvertToHex('Raw ' || to_char(i)));
        insert into &main_user..DemoStrings
        values (i, 'String ' || to_char(i), t_RawValue,
                'Fixed Char ' || to_char(i),
                decode(mod(i, 2), 0, null, 'Nullable ' || to_char(i)));
    end loop;
end;
/

begin
    for i in 1..10 loop
        insert into &main_user..DemoUnicodes
        values (i, 'Unicode ' || unistr('\3042') || ' ' || to_char(i),
                'Fixed Unicode ' || to_char(i),
                decode(mod(i, 2), 0, null, unistr('Nullable ') || to_char(i)));
    end loop;
end;
/

begin
    for i in 1..10 loop
        insert into &main_user..DemoDates
        values (i, to_date(20021209, 'YYYYMMDD') + i + i * .1,
                decode(mod(i, 2), 0, null,
                to_date(20021209, 'YYYYMMDD') + i + i + i * .15));
    end loop;
end;
/

begin
    for i in 1..10 loop
        insert into &main_user..DemoTimestamps
        values (i,
            to_timestamp('20021209', 'YYYYMMDD') +
                to_dsinterval(to_char(i) || ' 00:00:' ||
                to_char(i * 2) || '.' || to_char(i * 50)),
            to_timestamp_tz('20021210 00:00:00 ' ||
                    decode(mod(i, 2), 0, '-', '+') ||
                    ltrim(to_char(abs(i - 6), '00')) || ':' ||
                    decode(mod(i, 2), 0, '30', '00'),
                    'YYYYMMDD HH24:MI:SS TZH:TZM') +
                to_dsinterval(to_char(i) || ' 00:00:' ||
                    to_char(i * 3) || '.' || to_char(i * 75)),
            to_timestamp_tz('20021211 00:00:00 ' || to_char(i - 8, 'S00') ||
                    ':00', 'YYYYMMDD HH24:MI:SS TZH:TZM') +
                to_dsinterval(to_char(i) || ' 00:00:' ||
                    to_char(i * 4) || '.' || to_char(i * 100)),
            decode(mod(i, 2), 0, to_timestamp(null, 'YYYYMMDD'),
            to_timestamp('20021209', 'YYYYMMDD') +
                to_dsinterval(to_char(i + 1) || ' 00:00:' ||
                        to_char(i * 3) || '.' || to_char(i * 125))));
    end loop;
end;
/

begin
    for i in 1..10 loop
        insert into &main_user..DemoIntervals
        values (i, to_dsinterval(to_char(i) || ' ' || to_char(i) || ':' ||
                to_char(i * 2) || ':' || to_char(i * 3)),
                decode(mod(i, 2), 0, to_dsinterval(null),
                to_dsinterval(to_char(i + 5) || ' ' || to_char(i + 2) || ':' ||
                to_char(i * 2 + 5) || ':' || to_char(i * 3 + 5))));
    end loop;
end;
/

insert into &main_user..DemoObjects values (1,
    &main_user..udt_Object(1, 'First row', 'First',
        to_date(20070306, 'YYYYMMDD'),
        to_timestamp('20080912 16:40:00', 'YYYYMMDD HH24:MI:SS'),
        &main_user..udt_SubObject(11, 'Sub object 1'),
        &main_user..udt_ObjectArray(
                &main_user..udt_SubObject(5, 'first element'),
                &main_user..udt_SubObject(6, 'second element'))),
    &main_user..udt_Array(5, 10, null, 20));

insert into &main_user..DemoObjects values (2, null,
    &main_user..udt_Array(3, null, 9, 12, 15));

insert into &main_user..DemoObjects values (3,
    &main_user..udt_Object(3, 'Third row', 'Third',
        to_date(20070621, 'YYYYMMDD'),
        to_timestamp('20071213 07:30:45', 'YYYYMMDD HH24:MI:SS'),
        &main_user..udt_SubObject(13, 'Sub object 3'),
        &main_user..udt_ObjectArray(
                &main_user..udt_SubObject(10, 'element #1'),
                &main_user..udt_SubObject(20, 'element #2'),
                &main_user..udt_SubObject(30, 'element #3'),
                &main_user..udt_SubObject(40, 'element #4'))), null);

commit;

-- create procedures for demoing callproc()
create procedure &main_user..proc_Demo (
    a_InValue                           varchar2,
    a_InOutValue                        in out number,
    a_OutValue                          out number
) as
begin
    a_InOutValue := a_InOutValue * length(a_InValue);
    a_OutValue := length(a_InValue);
end;
/

create procedure &main_user..proc_DemoNoArgs as
begin
    null;
end;
/

-- create functions for demoing callfunc()
create function &main_user..func_Demo (
    a_String                            varchar2,
    a_ExtraAmount                       number
) return number as
begin
    return length(a_String) + a_ExtraAmount;
end;
/

create function &main_user..func_DemoNoArgs
return number as
begin
    return 712;
end;
/

-- create packages
create or replace package &main_user..pkg_DemoStringArrays as

    type udt_StringList is table of varchar2(100) index by binary_integer;

    function DemoInArrays (
        a_StartingLength                number,
        a_Array                         udt_StringList
    ) return number;

    procedure DemoInOutArrays (
        a_NumElems                      number,
        a_Array                         in out nocopy udt_StringList
    );

    procedure DemoOutArrays (
        a_NumElems                      number,
        a_Array                         out nocopy udt_StringList
    );

    procedure DemoIndexBy (
        a_Array                         out nocopy udt_StringList
    );

end;
/

create or replace package body &main_user..pkg_DemoStringArrays as

    function DemoInArrays (
        a_StartingLength                number,
        a_Array                         udt_StringList
    ) return number is
        t_Length                        number;
    begin
        t_Length := a_StartingLength;
        for i in 1..a_Array.count loop
            t_Length := t_Length + length(a_Array(i));
        end loop;
        return t_Length;
    end;

    procedure DemoInOutArrays (
        a_NumElems                      number,
        a_Array                         in out udt_StringList
    ) is
    begin
        for i in 1..a_NumElems loop
            a_Array(i) := 'Converted element # ' ||
                    to_char(i) || ' originally had length ' ||
                    to_char(length(a_Array(i)));
        end loop;
    end;

    procedure DemoOutArrays (
        a_NumElems                      number,
        a_Array                         out udt_StringList
    ) is
    begin
        for i in 1..a_NumElems loop
            a_Array(i) := 'Demo out element # ' || to_char(i);
        end loop;
    end;

    procedure DemoIndexBy (
        a_Array                         out nocopy udt_StringList
    ) is
    begin
        a_Array(-1048576) := 'First element';
        a_Array(-576) := 'Second element';
        a_Array(284) := 'Third element';
        a_Array(8388608) := 'Fourth element';
    end;

end;
/

create or replace package &main_user..pkg_DemoUnicodeArrays as

    type udt_UnicodeList is table of nvarchar2(100) index by binary_integer;

    function DemoInArrays (
        a_StartingLength                number,
        a_Array                         udt_UnicodeList
    ) return number;

    procedure DemoInOutArrays (
        a_NumElems                      number,
        a_Array                         in out nocopy udt_UnicodeList
    );

    procedure DemoOutArrays (
        a_NumElems                      number,
        a_Array                         out nocopy udt_UnicodeList
    );

end;
/

create or replace package body &main_user..pkg_DemoUnicodeArrays as

    function DemoInArrays (
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

    procedure DemoInOutArrays (
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

    procedure DemoOutArrays (
        a_NumElems          number,
        a_Array             out udt_UnicodeList
    ) is
    begin
        for i in 1..a_NumElems loop
            a_Array(i) := unistr('Demo out element ') ||
                    unistr('\3042') || ' # ' || to_char(i);
        end loop;
    end;

end;
/

create or replace package &main_user..pkg_DemoNumberArrays as

    type udt_NumberList is table of number index by binary_integer;

    function DemoInArrays (
        a_StartingValue                 number,
        a_Array                         udt_NumberList
    ) return number;

    procedure DemoInOutArrays (
        a_NumElems                      number,
        a_Array                         in out nocopy udt_NumberList
    );

    procedure DemoOutArrays (
        a_NumElems                      number,
        a_Array                         out nocopy udt_NumberList
    );

end;
/

create or replace package body &main_user..pkg_DemoNumberArrays as

    function DemoInArrays (
        a_StartingValue                 number,
        a_Array                         udt_NumberList
    ) return number is
        t_Value                         number;
    begin
        t_Value := a_StartingValue;
        for i in 1..a_Array.count loop
            t_Value := t_Value + a_Array(i);
        end loop;
        return t_Value;
    end;

    procedure DemoInOutArrays (
        a_NumElems                      number,
        a_Array                         in out udt_NumberList
    ) is
    begin
        for i in 1..a_NumElems loop
            a_Array(i) := a_Array(i) * 10;
        end loop;
    end;

    procedure DemoOutArrays (
        a_NumElems                      number,
        a_Array                         out udt_NumberList
    ) is
    begin
        for i in 1..a_NumElems loop
            a_Array(i) := i * 100;
        end loop;
    end;

end;
/

create or replace package &main_user..pkg_DemoDateArrays as

    type udt_DateList is table of date index by binary_integer;

    function DemoInArrays (
        a_StartingValue                 number,
        a_BaseDate                      date,
        a_Array                         udt_DateList
    ) return number;

    procedure DemoInOutArrays (
        a_NumElems                      number,
        a_Array                         in out nocopy udt_DateList
    );

    procedure DemoOutArrays (
        a_NumElems                      number,
        a_Array                         out nocopy udt_DateList
    );

end;
/

create or replace package body &main_user..pkg_DemoDateArrays as

    function DemoInArrays (
        a_StartingValue                 number,
        a_BaseDate                      date,
        a_Array                         udt_DateList
    ) return number is
        t_Value                         number;
    begin
        t_Value := a_StartingValue;
        for i in 1..a_Array.count loop
            t_Value := t_Value + a_Array(i) - a_BaseDate;
        end loop;
        return t_Value;
    end;

    procedure DemoInOutArrays (
        a_NumElems                      number,
        a_Array                         in out udt_DateList
    ) is
    begin
        for i in 1..a_NumElems loop
            a_Array(i) := a_Array(i) + 7;
        end loop;
    end;

    procedure DemoOutArrays (
        a_NumElems                      number,
        a_Array                         out udt_DateList
    ) is
    begin
        for i in 1..a_NumElems loop
            a_Array(i) := to_date(20021212, 'YYYYMMDD') + i * 1.2;
        end loop;
    end;

end;
/

create or replace package &main_user..pkg_DemoOutCursors as

    type udt_RefCursor is ref cursor;

    procedure DemoOutCursor (
        a_MaxIntValue                   number,
        a_Cursor                        out udt_RefCursor
    );

end;
/

create or replace package body &main_user..pkg_DemoOutCursors as

    procedure DemoOutCursor (
        a_MaxIntValue                   number,
        a_Cursor                        out udt_RefCursor
    ) is
    begin
        open a_Cursor for
            select
                IntCol,
                StringCol
            from DemoStrings
            where IntCol <= a_MaxIntValue
            order by IntCol;
    end;

end;
/

create or replace package &main_user..pkg_DemoBooleans as

    type udt_BooleanList is table of boolean index by binary_integer;

    function GetStringRep (
        a_Value                         boolean
    ) return varchar2;

    function IsLessThan10 (
        a_Value                         number
    ) return boolean;

    function DemoInArrays (
        a_Value                         udt_BooleanList
    ) return number;

    procedure DemoOutArrays (
        a_NumElements                   number,
        a_Value                         out nocopy udt_BooleanList
    );

end;
/

create or replace package body &main_user..pkg_DemoBooleans as

    function GetStringRep (
        a_Value                         boolean
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
        a_Value                         number
    ) return boolean is
    begin
        return a_Value < 10;
    end;

    function DemoInArrays (
        a_Value                         udt_BooleanList
    ) return number is
        t_Result                        pls_integer;
    begin
        t_Result := 0;
        for i in 1..a_Value.count loop
            if a_Value(i) then
                t_Result := t_Result + 1;
            end if;
        end loop;
        return t_Result;
    end;

    procedure DemoOutArrays (
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

create or replace package &main_user..pkg_DemoBindObject as

    function GetStringRep (
        a_Object                        udt_Object
    ) return varchar2;

end;
/

create or replace package body &main_user..pkg_DemoBindObject as

    function GetStringRep (
        a_Object                        udt_SubObject
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

create or replace package &main_user..pkg_DemoRecords as

    type udt_Record is record (
        NumberValue                     number,
        StringValue                     varchar2(30),
        DateValue                       date,
        TimestampValue                  timestamp,
        BooleanValue                    boolean
    );

    function GetStringRep (
        a_Value                         udt_Record
    ) return varchar2;

    procedure DemoOut (
        a_Value                         out nocopy udt_Record
    );

end;
/

create or replace package body &main_user..pkg_DemoRecords as

    function GetStringRep (
        a_Value                         udt_Record
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

    procedure DemoOut (
        a_Value                         out nocopy udt_Record
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

create or replace package &main_user..pkg_DemoLOBs as

    procedure DemoInOutTempClob (
        a_IntValue                      number,
        a_CLOB                          in out clob
    );

end;
/

create or replace package body &main_user..pkg_DemoLOBs as

    procedure DemoInOutTempClob (
        a_IntValue                      number,
        a_CLOB                          in out clob
    ) is
    begin

        delete from DemoClobs
        where IntCol = a_IntValue;

        insert into DemoClobs (
            IntCol,
            ClobCol
        ) values (
            a_IntValue,
            a_CLOB
        );

        select ClobCol
        into a_CLOB
        from DemoClobs
        where IntCol = a_IntValue;

    end;

end;
/

create or replace procedure &main_user..proc_DemoInOut (
    a_StringCol                         in out varchar2,
    a_UnicodeCol                        in out nvarchar2,
    a_FloatCol                          in out float,
    a_DoublePrecCol                     in out double precision,
    a_NumberCol                         in out number,
    a_DateCol                           in out date,
    a_TimestampCol                      in out timestamp,
    a_TimestampTZCol                    in out timestamp with time zone,
    a_IntervalDSCol                     in out interval day to second,
    a_IntervalYMCol                     in out interval year to month,
    a_BinaryFltCol                      in out binary_float,
    a_BinaryDoubleCol                   in out binary_double
) as
begin
    a_StringCol        := 'demostring';
    a_UnicodeCol       := 'demounicode';
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

-- create type and table for demoing advanced queuing with objects
create or replace type &main_user..udt_Book as object (
    Title                               varchar2(100),
    Authors                             varchar2(100),
    Price                               number(5,2)
);
/

-- create queues for demoing advanced queuing with objects and RAW
begin

    dbms_aqadm.create_queue_table('&main_user..BOOK_QUEUE_TAB',
            '&main_user..UDT_BOOK');
    dbms_aqadm.create_queue('&main_user..DEMO_BOOK_QUEUE',
            '&main_user..BOOK_QUEUE_TAB');
    dbms_aqadm.start_queue('&main_user..DEMO_BOOK_QUEUE');

    dbms_aqadm.create_queue_table('&main_user..RAW_QUEUE_TAB', 'RAW');
    dbms_aqadm.create_queue('&main_user..DEMO_RAW_QUEUE',
            '&main_user..RAW_QUEUE_TAB');
    dbms_aqadm.start_queue('&main_user..DEMO_RAW_QUEUE');

end;
/
