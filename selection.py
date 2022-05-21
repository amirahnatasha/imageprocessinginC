# Selection sort in python

def selectionSort(array, size):

    for step in range(size):
        min = step

        for i in range(step + 1, size):

            if(array[i] < array[min]):
                min  = i

        (array[step], array[min]) = (array[min], array[step])

arr = [-2, 45, 0, 11, -9]
size = len(arr)
selectionSort(arr, size)
print('Sorted array :')
print(arr)