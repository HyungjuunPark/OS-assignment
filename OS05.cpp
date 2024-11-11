#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <fstream>
#include <mutex>
#include <iomanip>

using namespace std;

constexpr int MAX_SIZE = 100; // 최대 행렬 크기
constexpr int MAX_MATRICES = 10; // 최대 행렬 개수
constexpr int RAND_MIN_VAL = -3; // 랜덤 값의 최소값
constexpr int RAND_MAX_VAL = 3; // 랜덤 값의 최대값
mutex mtx; // 파일 쓰기 시 스레드 안전을 위한 mutex

// 행렬 타입 정의
struct Matrix {
    int rows;
    int cols;
    int data[MAX_SIZE][MAX_SIZE];
};

// 주어진 행(row)과 열(col)을 가진 랜덤 행렬 생성 함수
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

    // 각 원소 계산을 위한 스레드 작업 정의
    auto worker = [&A, &B, &result](int i, int j) {
        result.data[i][j] = 0;
        for (int k = 0; k < A.cols; ++k)
            result.data[i][j] += A.data[i][k] * B.data[k][j];
        };

    // 각 원소를 스레드로 계산
    thread threads[MAX_SIZE * MAX_SIZE];
    int thread_index = 0;
    for (int i = 0; i < result.rows; ++i) {
        for (int j = 0; j < result.cols; ++j) {
            threads[thread_index++] = thread(worker, i, j);
        }
    }

    for (int i = 0; i < thread_index; ++i)
        threads[i].join(); // 모든 스레드가 완료될 때까지 대기

    return result;
}

// 행렬의 차원과 데이터를 출력하는 함수
void displayMatrix(const Matrix& matrix) {
    for (int i = 0; i < matrix.rows; ++i) {
        for (int j = 0; j < matrix.cols; ++j) {
            cout << setw(4) << matrix.data[i][j] << " ";
        }
        cout << endl;
    }
}

// 행렬을 파일에 기록하는 함수
void writeMatrixToFile(ofstream& file, const Matrix& matrix) {
    for (int i = 0; i < matrix.rows; ++i) {
        for (int j = 0; j < matrix.cols; ++j) {
            file << setw(4) << matrix.data[i][j] << " ";
        }
        file << endl;
    }
}

// 메인 함수
int main() {
    int num_matrices;
    cout << "곱셈할 행렬의 개수를 입력하세요 (최대 " << MAX_MATRICES << "): ";
    cin >> num_matrices;

    if (num_matrices < 2 || num_matrices > MAX_MATRICES) {
        cout << "잘못된 개수입니다. 최소 2개, 최대 " << MAX_MATRICES << "개의 행렬이 필요합니다." << endl;
        return 1;
    }

    Matrix matrices[MAX_MATRICES];
    for (int i = 0; i < num_matrices; ++i) {
        int rows, cols;
        cout << "행렬 " << i + 1 << "의 행과 열을 입력하세요 (예: 3 4): ";
        cin >> rows >> cols;

        matrices[i] = generateMatrix(rows, cols);
        cout << "생성된 행렬 " << i + 1 << " (" << rows << "x" << cols << "):" << endl;
        displayMatrix(matrices[i]);
    }

    auto start = chrono::high_resolution_clock::now();
    this_thread::sleep_for(chrono::milliseconds(1000)); // 계산 전 1초 대기

    Matrix result = matrices[0];
    ofstream file("output.txt");

    // 행렬 곱셈 순차적으로 수행
    for (int i = 1; i < num_matrices; ++i) {
        cout << "< " << result.rows << "x" << result.cols << " > X < "
            << matrices[i].rows << "x" << matrices[i].cols << " > = < "
            << result.rows << "x" << matrices[i].cols << " >" << endl;
        result = multiplyMatrices(result, matrices[i]);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "결과:" << endl;
    displayMatrix(result);

    cout << "처리 시간: " << fixed << setprecision(3) << duration.count() << " 초" << endl;

    file << "결과:" << endl;
    writeMatrixToFile(file, result);
    file << "처리 시간: " << fixed << setprecision(3) << duration.count() << " 초" << endl;
    file.close();

    return 0;
}
