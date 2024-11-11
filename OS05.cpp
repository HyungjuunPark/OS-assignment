#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <fstream>
#include <mutex>
#include <iomanip>

using namespace std;

constexpr int MAX_SIZE = 100; // �ִ� ��� ũ��
constexpr int MAX_MATRICES = 10; // �ִ� ��� ����
constexpr int RAND_MIN_VAL = -3; // ���� ���� �ּҰ�
constexpr int RAND_MAX_VAL = 3; // ���� ���� �ִ밪
mutex mtx; // ���� ���� �� ������ ������ ���� mutex

// ��� Ÿ�� ����
struct Matrix {
    int rows;
    int cols;
    int data[MAX_SIZE][MAX_SIZE];
};

// �־��� ��(row)�� ��(col)�� ���� ���� ��� ���� �Լ�
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

// �� ����� ���ϴ� �Լ�
Matrix multiplyMatrices(const Matrix& A, const Matrix& B) {
    Matrix result;
    result.rows = A.rows;
    result.cols = B.cols;

    // �� ���� ����� ���� ������ �۾� ����
    auto worker = [&A, &B, &result](int i, int j) {
        result.data[i][j] = 0;
        for (int k = 0; k < A.cols; ++k)
            result.data[i][j] += A.data[i][k] * B.data[k][j];
        };

    // �� ���Ҹ� ������� ���
    thread threads[MAX_SIZE * MAX_SIZE];
    int thread_index = 0;
    for (int i = 0; i < result.rows; ++i) {
        for (int j = 0; j < result.cols; ++j) {
            threads[thread_index++] = thread(worker, i, j);
        }
    }

    for (int i = 0; i < thread_index; ++i)
        threads[i].join(); // ��� �����尡 �Ϸ�� ������ ���

    return result;
}

// ����� ������ �����͸� ����ϴ� �Լ�
void displayMatrix(const Matrix& matrix) {
    for (int i = 0; i < matrix.rows; ++i) {
        for (int j = 0; j < matrix.cols; ++j) {
            cout << setw(4) << matrix.data[i][j] << " ";
        }
        cout << endl;
    }
}

// ����� ���Ͽ� ����ϴ� �Լ�
void writeMatrixToFile(ofstream& file, const Matrix& matrix) {
    for (int i = 0; i < matrix.rows; ++i) {
        for (int j = 0; j < matrix.cols; ++j) {
            file << setw(4) << matrix.data[i][j] << " ";
        }
        file << endl;
    }
}

// ���� �Լ�
int main() {
    int num_matrices;
    cout << "������ ����� ������ �Է��ϼ��� (�ִ� " << MAX_MATRICES << "): ";
    cin >> num_matrices;

    if (num_matrices < 2 || num_matrices > MAX_MATRICES) {
        cout << "�߸��� �����Դϴ�. �ּ� 2��, �ִ� " << MAX_MATRICES << "���� ����� �ʿ��մϴ�." << endl;
        return 1;
    }

    Matrix matrices[MAX_MATRICES];
    for (int i = 0; i < num_matrices; ++i) {
        int rows, cols;
        cout << "��� " << i + 1 << "�� ��� ���� �Է��ϼ��� (��: 3 4): ";
        cin >> rows >> cols;

        matrices[i] = generateMatrix(rows, cols);
        cout << "������ ��� " << i + 1 << " (" << rows << "x" << cols << "):" << endl;
        displayMatrix(matrices[i]);
    }

    auto start = chrono::high_resolution_clock::now();
    this_thread::sleep_for(chrono::milliseconds(1000)); // ��� �� 1�� ���

    Matrix result = matrices[0];
    ofstream file("output.txt");

    // ��� ���� ���������� ����
    for (int i = 1; i < num_matrices; ++i) {
        cout << "< " << result.rows << "x" << result.cols << " > X < "
            << matrices[i].rows << "x" << matrices[i].cols << " > = < "
            << result.rows << "x" << matrices[i].cols << " >" << endl;
        result = multiplyMatrices(result, matrices[i]);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "���:" << endl;
    displayMatrix(result);

    cout << "ó�� �ð�: " << fixed << setprecision(3) << duration.count() << " ��" << endl;

    file << "���:" << endl;
    writeMatrixToFile(file, result);
    file << "ó�� �ð�: " << fixed << setprecision(3) << duration.count() << " ��" << endl;
    file.close();

    return 0;
}
