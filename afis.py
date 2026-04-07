import ctypes
import os

# Load the shared library
lib_path = os.path.join(os.path.dirname(__file__), 'libafis.so')
lib = ctypes.CDLL(lib_path)

# Define function signatures
lib.match_jiang_files.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
lib.match_jiang_files.restype = ctypes.c_float

lib.match_mcc_files.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
lib.match_mcc_files.restype = ctypes.c_float

lib.match_jiang_csv.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
lib.match_jiang_csv.restype = ctypes.c_float

lib.match_mcc_csv.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
lib.match_mcc_csv.restype = ctypes.c_float

lib.search_jiang_db_csv.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_int)]
lib.search_jiang_db_csv.restype = ctypes.c_float

lib.search_mcc_db_csv.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_int)]
lib.search_mcc_db_csv.restype = ctypes.c_float

def match_jiang_files(file1, file2):
    """
    Match two fingerprints using Jiang algorithm from .xyt files.
    Returns match score (0-100), or -1 on error.
    """
    return lib.match_jiang_files(file1.encode('utf-8'), file2.encode('utf-8'))

def match_mcc_files(file1, file2):
    """
    Match two fingerprints using MCC algorithm from .xyt files.
    Returns match score (0-100), or -1 on error.
    """
    return lib.match_mcc_files(file1.encode('utf-8'), file2.encode('utf-8'))

def match_jiang_csv(csv1, csv2):
    """
    Match two fingerprints using Jiang algorithm from CSV strings.
    CSV format: each line "x,y,theta[,quality]" (quality optional)
    Returns match score (0-100), or -1 on error.
    """
    return lib.match_jiang_csv(csv1.encode('utf-8'), csv2.encode('utf-8'))

def match_mcc_csv(csv1, csv2):
    """
    Match two fingerprints using MCC algorithm from CSV strings.
    CSV format: each line "x,y,theta[,quality]" (quality optional)
    Returns match score (0-100), or -1 on error.
    """
    return lib.match_mcc_csv(csv1.encode('utf-8'), csv2.encode('utf-8'))

# For DPDDFF and genericMatching, they are parallel search algorithms.
# Since the library is sequential, you can implement search in Python.
# For example:
def search_jiang_files(query_file, db_files):
    """
    Search for the best match in a database using Jiang algorithm.
    db_files: list of .xyt files
    Returns (best_score, best_file)
    """
    best_score = -1
    best_file = None
    for db_file in db_files:
        score = match_jiang_files(query_file, db_file)
        if score > best_score:
            best_score = score
            best_file = db_file
    return best_score, best_file

def search_mcc_files(query_file, db_files):
    """
    Search for the best match in a database using MCC algorithm.
    db_files: list of .xyt files
    Returns (best_score, best_file)
    """
    best_score = -1
    best_file = None
    for db_file in db_files:
        score = match_mcc_files(query_file, db_file)
        if score > best_score:
            best_score = score
            best_file = db_file
    return best_score, best_file

def search_jiang_csv(query_csv, db_csvs):
    """
    Search for the best match in a database using Jiang algorithm.
    db_csvs: list of CSV strings
    Returns (best_score, best_csv_index)
    """
    best_score = -1
    best_index = -1
    for i, db_csv in enumerate(db_csvs):
        score = match_jiang_csv(query_csv, db_csv)
        if score > best_score:
            best_score = score
            best_index = i
    return best_score, best_index

def search_mcc_csv(query_csv, db_csvs):
    """
    Search for the best match in a database using MCC algorithm.
    db_csvs: list of CSV strings
    Returns (best_score, best_csv_index)
    """
    best_score = -1
    best_index = -1
    for i, db_csv in enumerate(db_csvs):
        score = match_mcc_csv(query_csv, db_csv)
        if score > best_score:
            best_score = score
            best_index = i
    return best_score, best_index

def search_jiang_db_csv(db_csv, query_csv):
    """
    Search in a single database CSV using Jiang algorithm.
    db_csv: CSV string with format id,x,y,theta[,quality]
    query_csv: CSV string for query fingerprint
    Returns (best_score, best_id)
    """
    best_id = ctypes.c_int()
    score = lib.search_jiang_db_csv(db_csv.encode('utf-8'), query_csv.encode('utf-8'), ctypes.byref(best_id))
    return score, best_id.value

def search_mcc_db_csv(db_csv, query_csv):
    """
    Search in a single database CSV using MCC algorithm.
    db_csv: CSV string with format id,x,y,theta[,quality]
    query_csv: CSV string for query fingerprint
    Returns (best_score, best_id)
    """
    best_id = ctypes.c_int()
    score = lib.search_mcc_db_csv(db_csv.encode('utf-8'), query_csv.encode('utf-8'), ctypes.byref(best_id))
    return score, best_id.value