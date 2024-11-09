/*=====================================================================*
 *                 do_longest_path_to_parentless                       *
 *=====================================================================*/

PUBLIC void longest_path_to_parentless(int* max_path_length, pid_t* max_path_pid, pid_t excluded_pid)
PUBLIC int is_parentless(pid_t pid);
PUBLIC int determine_path_length(int proc_nr);

PUBLIC int do_who_longest_path_to_parentless(pid_t excluded_pid)
{
  int path_length = -1;
  pid_t who = -1;
  longest_path_to_parentless(&path_length, &who, excluded_pid);
  return who;
}

PUBLIC int do_longest_path_to_parentless(pid_t excluded_pid)
{
  int path_length = -1;
  pid_t who = -1;
  longest_path_to_parentless(&path_length, &who, excluded_pid);
  return path_length;
}

PUBLIC void longest_path_to_parentless(int* max_path_length, pid_t* max_path_pid, pid_t excluded_pid)
{

  /* Iterate through process descriptor table
  // and find the one that is not parent of any

  // For every parentless process go up through
  // the mproc table and count length
  // of the path */

  pid_t curr_proc_pid;
  int curr_path_length;
  int proc_nr;

  for (proc_nr = 0; proc_nr < NR_PROCS; ++proc_nr)
  {
        curr_proc_pid = mproc[proc_nr].mp_pid;

        if (curr_proc_pid == excluded_pid) continue;

        if (is_parentless(curr_proc_pid))
        {
                curr_path_length = determine_path_length(proc_nr);
                if (curr_path_length > max_path_length)
                {
                        *max_path_length = curr_path_length;
                        *max_path_pid = curr_proc_pid;
                }
        }
  }
}

PUBLIC int is_parentless(pid_t pid)
{
  int proc_nr;

  for (proc_nr = 0; proc_nr < NR_PROCS; ++proc_nr)
  {
        if ((mproc[proc_nr].mp_flags & IN_USE) && mproc[proc_nr].mp_pid != pid
                && mproc[mproc[proc_nr].m_parent] == pid)

        { return 0; } /* it is not parentless */
  }

  return 1;
}

PUBLIC int determine_path_length(int proc_nr)
{
  int path_length = 0;
  int curr_proc_pid = mproc[proc_nr].mp_pid;
  pid_t parent_pid;
  int parent_idx;

  while (curr_proc_pid != 0)
  {
        parent_idx = mproc[proc_nr].mp_parent;
        if (parent_idx == 0) { break; }

        parent_pid = mproc[parent_idx].mp_pid;
        curr_proc_pid = parent_pid;
        proc_nr = parent_idx;

        ++path_length;
  }

  return path_length;
}