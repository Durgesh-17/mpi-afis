#ifndef WRAPPER_H
#define WRAPPER_H

extern "C" {
float match_jiang_csv(const char* csv1, const char* csv2);
float match_mcc_csv(const char* csv1, const char* csv2);
float match_jiang_files(const char* file1, const char* file2);
float match_mcc_files(const char* file1, const char* file2);
float search_jiang_db_csv(const char* db_csv, const char* query_csv, int* best_id);
float search_mcc_db_csv(const char* db_csv, const char* query_csv, int* best_id);
}

#endif