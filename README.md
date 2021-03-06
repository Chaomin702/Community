# Community

- [x] merging process 

- [x] restoring process

- [x] no-overlapping community based algorithm

- [x] overlapping community based algorithm

- [ ] distributed set-cover algorithm

### Use case

    cd ./Test
    python test.py

### Example

k = 8, n = 100

 ![res](res.png)

| Algorithm | $$ \tau(S,V)$$ |           $S$            |
| :-------: | :------------: | :----------------------: |
| Community |     3.959      | 18,76,69,100,22,12,17,33 |
|   Naive   |     6.538      |                          |

### Procedure

```
communities size: 5
id  centre  time  diffusion-nodes
1    22   5.473    
2    76   2.095    
3    54   1.675    
4    18   2.583    
5    69   1.662    

merging process...


1.
min R(C) id: 5   neightbors: 1  
5+1->6
communities size: 4
id  centre  time  diffusion-nodes
2    76   2.095    
3    54   1.675    
4    18   2.583    
6    22   5.473    

2.
min R(C) id: 3   neightbors: 4  6  
3+4->7
communities size: 3
id  centre  time  diffusion-nodes
2    76   2.095    
6    22   5.473    
7    18   2.583    

3.
min R(C) id: 2   neightbors: 6  
2+6->8
communities size: 2
id  centre  time  diffusion-nodes
7    18   2.583    
8    22   5.473    

4.
min R(C) id: 7   neightbors: 8  
7+8->9
communities size: 1
id  centre  time  diffusion-nodes
9    22   5.473    

5.
min R(C) id: 9   neightbors: 

restoring process...


9->7+8
communities size: 2
id  centre  time  diffusion-nodes
7    18   2.583    18 
8    22   5.473    22 

8->2+6
communities size: 3
id  centre  time  diffusion-nodes
7    18   2.583    18 
2    76   2.095    76 
6    22   5.473    22 

6->5+1
communities size: 4
id  centre  time  diffusion-nodes
7    18   2.583    18 
2    76   2.095    76 
5    69   1.662    69 
1    22   5.473    22 
communities size: 4
id  centre  time  diffusion-nodes
7    18   2.583    18 
2    76   2.095    76 
5    69   1.662    69 
1    22   4.774    100 22 
communities size: 4
id  centre  time  diffusion-nodes
7    18   2.583    18 
2    76   2.095    76 
5    69   1.662    69 
1    22   4.05     100 22 12 
communities size: 4
id  centre  time  diffusion-nodes
7    18   2.583    18 
2    76   2.095    76 
5    69   1.662    69 
1    22   4.05     100 22 12 17 
communities size: 4
id  centre  time  diffusion-nodes
7    18   2.583    18 
2    76   2.095    76 
5    69   1.662    69 
1    22   3.959    100 22 12 17 33 
min diffusion time: 3.959
result list: (community id: node id)
7 : 18 
2 : 76 
5 : 69 
1 : 100 
1 : 22 
1 : 12 
1 : 17 
1 : 33 
naive algorithm result: 6.538

```