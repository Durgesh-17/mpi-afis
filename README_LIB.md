# mpi-afis Shared Library

This is a shared library version of mpi-afis for fingerprint matching.

## Building on Linux

1. Ensure you have g++ and OpenMP installed.
2. Run `make -f Makefile.lib` to build libafis.so

## Usage

Use the Python wrapper afis.py to access the functions.

### Database CSV Format
For database search, the CSV should have format: `id,x,y,theta[,quality]` where `id` is the fingerprint identifier.

Example:
```
1,10,20,30,80
1,15,25,35,85
2,11,21,31,81
2,16,26,36,86
```

### Examples

```python
from afis import match_jiang_csv, match_mcc_csv, search_jiang_db_csv, search_mcc_db_csv

# Match two fingerprints from CSV
csv1 = "10,20,30\n15,25,35\n20,30,40"
csv2 = "11,21,31\n16,26,36\n21,31,41"
score = match_jiang_csv(csv1, csv2)
print(f"Jiang match score: {score}")

score = match_mcc_csv(csv1, csv2)
print(f"MCC match score: {score}")

# Search in a single database CSV (equivalent to DPDDFF/genericMatching)
db_csv = """1,10,20,30
1,15,25,35
2,11,21,31
2,16,26,36"""
query_csv = "12,22,32\n17,27,37"
best_score, best_id = search_jiang_db_csv(db_csv, query_csv)
print(f"Best match ID: {best_id}, score: {best_score}")

best_score, best_id = search_mcc_db_csv(db_csv, query_csv)
print(f"Best match ID: {best_id}, score: {best_score}")

# Still supports .xyt files if needed
from afis import match_jiang_files, search_jiang_files
score = match_jiang_files('example_fps/1_1.xyt', 'example_fps/1_2.xyt')
db_files = ['example_fps/1_1.xyt', 'example_fps/2_1.xyt']
best_score, best_file = search_jiang_files('example_fps/1_2.xyt', db_files)
```

## Functions

- match_jiang_csv(csv1, csv2): Match using Jiang algorithm from CSV strings
- match_mcc_csv(csv1, csv2): Match using MCC algorithm from CSV strings
- match_jiang_files(file1, file2): Match using Jiang from .xyt files
- match_mcc_files(file1, file2): Match using MCC from .xyt files
- search_jiang_csv(query_csv, db_csvs): Sequential search using Jiang with list of CSV strings
- search_mcc_csv(query_csv, db_csvs): Sequential search using MCC with list of CSV strings
- search_jiang_db_csv(db_csv, query_csv): Search in single database CSV using Jiang
- search_mcc_db_csv(db_csv, query_csv): Search in single database CSV using MCC
- search_jiang_files(query_file, db_files): Sequential search using Jiang with files
- search_mcc_files(query_file, db_files): Sequential search using MCC with files

The DPDDFF and genericMatching are parallel search algorithms, but here we provide sequential equivalents using the core matchers (Jiang and MCC).