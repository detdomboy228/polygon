import numpy as np


# ЗАДАЧА 1

def find_count_of_inclusions(array):
    """
    :param array: Массив, в котором будет подсчитано количество вхождений каждого его элемента в него самого
    :return new_array: Итоговый массив, i-й элемент которого - кол-во вхождений i-го элемента array в массив array
    """
    # Получаем индексы элементов array в массиве уникальных значений array
    # (работа с уникальными элементами позволит не бояться огромных чисел, так что это плюс)
    _, indices = np.unique(array, return_inverse=True)

    # Создаем np-массив от индексов с помощью bincount, это позволяет "сжать" новый массив до минимальной длины
    # Его длина - кол-во уникальных значений в array
    bincount_array = np.bincount(indices)

    # Проиндексируем массив bincount_array по indices, чтобы получить финальный результат
    # (выбираем элементы из bincount_array по индексам indices)
    new_array = bincount_array[indices]

    return list(map(int, new_array))


# Тестирующая функция
def test():
    inputs = [[1, 2, 1, 5, 1, 2], [1, 0, 1], [4, 3, 9, 18, 3, 7, 3, 100, 8, 7], [732000079675], []]
    answers = [[3, 2, 3, 1, 3, 2], [2, 1, 2], [1, 3, 1, 1, 3, 2, 3, 1, 1, 2], [1], []]
    for i in range(len(inputs)):
        assert find_count_of_inclusions(inputs[i]) == answers[i]


if __name__ == "__main__":
    test()

    # В качестве примера проверим действие алгоритма на одном массиве
    array = [3, 6, 9, 187, 187, 9, 8, 1, 4, 5, 187, 8, 1, 9, 6, 1, 3, 1, 4]
    new_array = find_count_of_inclusions(array)

    print("Было:", array)
    print("Стало:", new_array)