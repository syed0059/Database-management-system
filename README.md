# Database Management System

A miniature database management system that allows storage and retrieval of data, as well as creation of a B+ Tree index on a field

## Contributors

<div align="center">
    <table>
        <tbody>
            <tr>
                <th>Profile</th>
                <td><a href='https://github.com/syed0059' title='Syed'> <img src='https://github.com/syed0059.png' height='50' width='50'/></a></td>
                <td><a href='https://github.com/axwhyzee' title='Wee Hung'> <img src='https://github.com/axwhyzee.png' height='50' width='50'/></a></td>
                <td><a href='https://github.com/Xuanniee' title='Xuan Yi'> <img src='https://github.com/Xuanniee.png' height='50' width='50'/></a></td>
                <td><a href='https://github.com/heypeiyee' title='Pei Yee'> <img src='https://github.com/heypeiyee.png' height='50' width='50'/></a></td>
            </tr>
            <tr>
                <th>Name</th>
                <td>Syed</td>
                <td>Wee Hung</td>
                <td>Xuan Yi</td>
                <td>Pei Yee</td>
            </tr>
        </tbody>
    </table>
</div>

## Installation guide
1. Ensure that your computer has the g++ compiler installed before running our code. You may check if the compiler exists by following the steps below:
- On Mac:
```
g++ --version
```
- On Windows:
```
g++ -v
```

2. git clone this repository to copy the source code.

3. Run the code in a single line in your terminal after git cloning the source code.
```
g++ -std=c++17 "Block Manager/blockManager.cpp" "BPlusTree/bPlusTree.cpp" "BPlusTree/utils.cpp" "Node/node.cpp" "loadData.cpp" "main.cpp" -o main;
```

If no errors arise then the main program can be run using ```./main```
