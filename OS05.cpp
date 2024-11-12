#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <mutex>
#include <Windows.h>

using namespace std;

constexpr int MAX_SIZE = 100; // 최대 행렬 크기
constexpr int RAND_MIN_VAL = -3; // 랜덤 값의 최소값
constexpr int RAND_MAX_VAL = 3; // 랜덤 값의 최대값
mutex mtx;

// 행렬 타입 정의
struct Matrix {
    int rows;
    int cols;
    int data[MAX_SIZE][MAX_SIZE];
};

// 주어진 행과 열을 가진 랜덤 행렬 생성 함수
Matrix generateMatrix(int rows, int cols) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(RAND_MIN_VAL, RAND_MAX_VAL);

    Matrix matrix;
    matrix.rows = rows;
    matrix.cols = cols;
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            matrix.data[i][j] = dis(gen);

    return matrix;
}

// 두 행렬을 곱하는 함수
Matrix multiplyMatrices(const Matrix& A, const Matrix& B) {
    Matrix result;
    result.rows = A.rows;
    result.cols = B.cols;

    // 각 항목을 별도 스레드에서 계산
    auto worker = [&A, &B, &result](int i, int j) {
        Sleep(1000); // 1초 대기
        result.data[i][j] = 0;
        for (int k = 0; k < A.cols; ++k) {
            result.data[i][j] += A.data[i][k] * B.data[k][j];
        }
        };

    vector<thread> threads;
    for (int i = 0; i < result.rows; ++i) {
        for (int j = 0; j < result.cols; ++j) {
            threads.push_back(thread(worker, i, j));
        }
    }
    for (auto& t : threads) {
        t.join();
    }

    return result;
}

// 행렬의 차원과 데이터를 출력하는 함수
void displayMatrix(const Matrix& matrix) {
    for (int i = 0; i < matrix.rows; ++i) {
        for (int j = 0; j < matrix.cols; ++j) {
            cout << setw(4) << matrix.data[i][j] << '\t';
        }
        cout << '\n';
    }
}

// 행렬을 파일에 기록하는 함수
void writeMatrixToFile(ofstream& file, const Matrix& matrix) {
    for (int i = 0; i < matrix.rows; ++i) {
        for (int j = 0; j < matrix.cols; ++j) {
            file << setw(4) << matrix.data[i][j] << '\t';
        }
        file << '\n';
    }
}

// 메인 함수
int main() {
    string input;
    getline(cin, input);
    istringstream iss(input);

    vector<Matrix> matrices;
    string dimension;
    while (iss >> dimension) {
        size_t x_pos = dimension.find('x');
        int rows = stoi(dimension.substr(0, x_pos));
        int cols = stoi(dimension.substr(x_pos + 1));
        matrices.push_back(generateMatrix(rows, cols));
    }

    auto start = chrono::high_resolution_clock::now();

    Matrix result = matrices[0];
    ofstream file("result.txt");

    cout << '\n';
    // 행렬 곱셈 순차적으로 수행
    for (size_t i = 1; i < matrices.size(); ++i) {
        cout << "<  " << result.rows << "x" << result.cols << "  > X <  "
            << matrices[i].rows << "x" << matrices[i].cols << "  > = <  "
            << result.rows << "x" << matrices[i].cols << "  >" << '\n';
        result = multiplyMatrices(result, matrices[i]);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << '\n';
    displayMatrix(result);
    cout << '\n';
    cout << "Processing time : " << fixed << setprecision(3) << duration.count() << " sec" << '\n';

    writeMatrixToFile(file, result);
    file << "Processing time : " << fixed << setprecision(3) << duration.count() << " sec" << '\n';
    file.close();

    return 0;
}
