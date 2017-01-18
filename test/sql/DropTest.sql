/*-----------------------------------------------------------------------------
 * Copyright (c) 2016 Oracle and/or its affiliates.  All rights reserved.
 * This program is free software: you can modify it and/or redistribute it
 * under the terms of:
 *
 * (i)  the Universal Permissive License v 1.0 or at your option, any
 *      later version (http://oss.oracle.com/licenses/upl); and/or
 *
 * (ii) the Apache License v 2.0. (http://www.apache.org/licenses/LICENSE-2.0)
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * DropTest.sql
 *   Drops the users named "DPIC" and "DPIC_PROXY".
 *---------------------------------------------------------------------------*/

whenever sqlerror exit failure

-- drop existing users, if present
begin
  for r in
      ( select username
        from dba_users
        where username in ('DPIC', 'DPIC_PROXY')
      ) loop
    execute immediate 'drop user ' || r.username || ' cascade';
  end loop;
end;
/

