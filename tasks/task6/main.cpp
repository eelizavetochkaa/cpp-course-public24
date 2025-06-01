/**
 * @file main.cpp
 * @brief задача написать программу, которая будет высчитывать значение интеграла через метод трапеций
 * Особенность этой задачи: использовать потоки(threads) для ускорения вычислений (нужно разделить задачу для вычисления части интеграла на каждый поток)
 *
 * на вход программе при запуске(это в argv[]) подается 3 числа: a, b, n, tn где [a, b] - интервал (целочисленный, неотрицательный и не больше 50 для задачи), n - количество разбиений, tn - threads number - количество потоков для подсчёта
 * примечание: n - количество разбиений в тестах будет явно нацело делиться на tn - количество потоков.
 *
 * на выход программе выведите значение интеграла через метод трапеций (вывод в стандартный поток std::cout, в конце выведите '\n'), вывод в поток с точностью до 4 знака
 * в функции trapezoidalIntegral есть переменная const std::function<double (double)> &f, при вызове подсчётов передайте ссылку на функцию из задания
 * реализовать подсчёт интеграла S(a, b) = (1+e^x)^0.5 dx
 * 
 *
 * литература:
 * https://ru.wikipedia.org/wiki/Метод_трапеций
 * https://habr.com/ru/articles/420867/
 */

#include <cmath>
#include <functional>
#include <numeric>
#include <vector>
#include <thread>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <mutex>


class Integral {
private:
    int a, b, n, tn;
    double result;
    std::mutex result_mutex;
public:
    Integral(int& argc, char** argv) {
        if (argc != 5) {
            throw std::invalid_argument("Wrong number of arguments");
        }
        a  = std::stoi(argv[1]);
        b  = std::stoi(argv[2]);
        n  = std::stoi(argv[3]);
        tn = std::stoi(argv[4]);
        result = 0.0;
    }


    static double integralFunction(double x) {
        return std::sqrt(1 + std::exp(x));
    }


    double calculateIntegral() {
        std::vector<std::thread> threads;
        double h = static_cast<double>(b - a) / n;
        int chunk_size = n / tn;

        for (int i = 0; i < tn; ++i) {
            threads.emplace_back([this, h, chunk_size, i]() {
                int start = i * chunk_size;
                int end = (i == tn - 1) ? n : start + chunk_size;
                double partial_sum = 0.0;

                for (int j = start; j < end; ++j) {
                    double x1 = a + j * h;
                    double x2 = a + (j + 1) * h;
                    partial_sum += (integralFunction(x1) + integralFunction(x2)) * h / 2.0;
                }

                std::lock_guard<std::mutex> lock(result_mutex);
                result += partial_sum;
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        return result;
    }

};



int main(int argc, char** argv)
{
    Integral i(argc, argv);
    std::cout << std::fixed;
    std::cout << i.calculateIntegral() << std::endl;
    return 0;
}