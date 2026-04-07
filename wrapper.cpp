#include "MatcherJiang/FingerprintJiang.h"
#include "MCC/MCC.h"
#include "include/Matrix.h"
#include "include/Minutia.h"
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <map>

Matrix<int> parse_csv(const std::string& csv) {
    std::vector<std::vector<int>> data;
    std::stringstream ss(csv);
    std::string line;
    while (std::getline(ss, line)) {
        std::stringstream line_ss(line);
        std::string cell;
        std::vector<int> row;
        while (std::getline(line_ss, cell, ',')) {
            row.push_back(std::stoi(cell));
        }
        if (row.size() >= 3) {  // x, y, theta, optional quality
            data.push_back(row);
        }
    }
    if (data.empty()) return Matrix<int>();
    Matrix<int> xyt(data.size(), 3);
    for (size_t i = 0; i < data.size(); ++i) {
        xyt[i][0] = data[i][0];  // x
        xyt[i][1] = data[i][1];  // y
        xyt[i][2] = data[i][2];  // theta
    }
    return xyt;
}

static void fill_fingerprint_from_matrix(Fingerprint &fp, const Matrix<int> &xyt) {
    std::vector<Minutia> minutiae;
    minutiae.reserve(xyt.rows());

    for (int i = 0; i < xyt.rows(); ++i) {
        minutiae.emplace_back(i, xyt[i][0], xyt[i][1], xyt[i][2] * 2, 100, OTH);
    }

    fp.setMinutiae(minutiae);
}

// Parse database CSV: assume format id,x,y,theta[,quality]
std::map<int, std::vector<std::vector<int>>> parse_db_csv(const std::string& csv) {
    std::map<int, std::vector<std::vector<int>>> db;
    std::stringstream ss(csv);
    std::string line;
    while (std::getline(ss, line)) {
        std::stringstream line_ss(line);
        std::string cell;
        std::vector<int> row;
        while (std::getline(line_ss, cell, ',')) {
            row.push_back(std::stoi(cell));
        }
        if (row.size() >= 4) {  // id, x, y, theta, optional quality
            int id = row[0];
            std::vector<int> minutia = {row[1], row[2], row[3]};
            db[id].push_back(minutia);
        }
    }
    return db;
}

extern "C" {

float match_jiang_csv(const char* csv1, const char* csv2) {
    Matrix<int> xyt1 = parse_csv(std::string(csv1));
    if (xyt1.rows() == 0) return -1.0f;
    FingerprintJiang fp1;
    fill_fingerprint_from_matrix(fp1, xyt1);
    fp1.initialize();

    Matrix<int> xyt2 = parse_csv(std::string(csv2));
    if (xyt2.rows() == 0) return -1.0f;
    FingerprintJiang fp2;
    fill_fingerprint_from_matrix(fp2, xyt2);
    fp2.initialize();

    return fp1.match(fp2);
}

float match_mcc_csv(const char* csv1, const char* csv2) {
    Matrix<int> xyt1 = parse_csv(std::string(csv1));
    if (xyt1.rows() == 0) return -1.0f;
    MCC fp1;
    fill_fingerprint_from_matrix(fp1, xyt1);
    fp1.initialize();

    Matrix<int> xyt2 = parse_csv(std::string(csv2));
    if (xyt2.rows() == 0) return -1.0f;
    MCC fp2;
    fill_fingerprint_from_matrix(fp2, xyt2);
    fp2.initialize();

    return fp1.match(fp2);
}

float match_jiang_files(const char* file1, const char* file2) {
    FingerprintJiang fp1;
    if(fp1.readFile(std::string(file1)) != 0) return -1.0f;
    fp1.initialize();
    FingerprintJiang fp2;
    if(fp2.readFile(std::string(file2)) != 0) return -1.0f;
    fp2.initialize();
    return fp1.match(fp2);
}

float match_mcc_files(const char* file1, const char* file2) {
    MCC fp1;
    if(fp1.readFile(std::string(file1)) != 0) return -1.0f;
    fp1.initialize();
    MCC fp2;
    if(fp2.readFile(std::string(file2)) != 0) return -1.0f;
    fp2.initialize();
    return fp1.match(fp2);
}

// Search in database CSV using Jiang
float search_jiang_db_csv(const char* db_csv, const char* query_csv, int* best_id) {
    auto db = parse_db_csv(std::string(db_csv));
    Matrix<int> query_xyt = parse_csv(std::string(query_csv));
    if (query_xyt.rows() == 0) return -1.0f;
    FingerprintJiang query_fp;
    fill_fingerprint_from_matrix(query_fp, query_xyt);
    query_fp.initialize();

    float best_score = -1.0f;
    int best = -1;
    for (const auto& pair : db) {
        int id = pair.first;
        const auto& minutiae = pair.second;
        if (minutiae.empty()) continue;
        Matrix<int> xyt(minutiae.size(), 3);
        for (size_t i = 0; i < minutiae.size(); ++i) {
            xyt[i][0] = minutiae[i][0];
            xyt[i][1] = minutiae[i][1];
            xyt[i][2] = minutiae[i][2];
        }
        FingerprintJiang fp;
        fill_fingerprint_from_matrix(fp, xyt);
        fp.initialize();
        float score = fp.match(query_fp);
        if (score > best_score) {
            best_score = score;
            best = id;
        }
    }
    if (best_id) *best_id = best;
    return best_score;
}

// Search in database CSV using MCC
float search_mcc_db_csv(const char* db_csv, const char* query_csv, int* best_id) {
    auto db = parse_db_csv(std::string(db_csv));
    Matrix<int> query_xyt = parse_csv(std::string(query_csv));
    if (query_xyt.rows() == 0) return -1.0f;
    MCC query_fp;
    fill_fingerprint_from_matrix(query_fp, query_xyt);
    query_fp.initialize();

    float best_score = -1.0f;
    int best = -1;
    for (const auto& pair : db) {
        int id = pair.first;
        const auto& minutiae = pair.second;
        if (minutiae.empty()) continue;
        Matrix<int> xyt(minutiae.size(), 3);
        for (size_t i = 0; i < minutiae.size(); ++i) {
            xyt[i][0] = minutiae[i][0];
            xyt[i][1] = minutiae[i][1];
            xyt[i][2] = minutiae[i][2];
        }
        MCC fp;
        fill_fingerprint_from_matrix(fp, xyt);
        fp.initialize();
        float score = fp.match(query_fp);
        if (score > best_score) {
            best_score = score;
            best = id;
        }
    }
    if (best_id) *best_id = best;
    return best_score;
}

}