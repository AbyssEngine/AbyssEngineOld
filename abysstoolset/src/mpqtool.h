#ifndef ABYSSTOOLSET_MPQTOOL_H
#define ABYSSTOOLSET_MPQTOOL_H

int mpqtool_run(int argc, char **argv);
int mpqtool_list(const char *mpq_path);
int mpqtool_extract(const char *mpq_path, char*file_path);

#endif // ABYSSTOOLSET_MPQTOOL_H
