#insertion sort in Python

def insertionSort(arr):

    for i in range(1, len(arr)):

        key = arr[i] #start of unsorted array

        j = i  - 1

        #move element bigger than key one postion to left
        while j >= 0 and key < arr[j] :
            arr[j+1] = arr[j]
            j -= 1
        arr[j+1] = key

arr = [12, 9, 23, 2, 19]
insertionSort(arr)
print ("Sorted array is : ")
print(arr)
