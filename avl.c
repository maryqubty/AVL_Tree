#include "avl.h"
#include <stdio.h>
#include <stdlib.h>

/****
  Student1 name: Mary Qubty
  Student2 name: Rashad Altouri

  Student1 ID: 206528275
  Student2 ID: 209424340
****/


/* Optional utility functions*/
int util_max(int x,int y)
{
    return x>y?x:y;
}
int util_min(int x,int y)
{
    return x>y?y:x;
}
int util_abs(int x)
{
    return (x>0)?x:-x;
}
//additional functions' print:
void creat_node(AVLNodePtr son, int x);
void update_nodes(AVLNodePtr root, int x, int height_upd, int sum);
AVLNodePtr balance(AVLNodePtr root);
AVLNodePtr rotate_right(AVLNodePtr root, AVLNodePtr tmp_ptr);
AVLNodePtr rotate_left(AVLNodePtr root, AVLNodePtr tmp_ptr);
AVLNodePtr neutral_rotation(AVLNodePtr root, AVLNodePtr tmp_ptr, int i, int j);
void copy(AVLNodePtr son, AVLNodePtr root);
AVLNodePtr successor(AVLNodePtr root, int x);
int update_height(AVLNodePtr root, int x, int height_upd);
AVLNodePtr update(AVLNodePtr root, int x, int height_upd, int sum);
int right_sum(AVLNodePtr lca_tmp);
int left_sum(AVLNodePtr lca_tmp);
int update_sum_right(AVLNodePtr lca_tmp, int s, int sum);
int update_sum_left(AVLNodePtr lca_tmp, int s, int sum);
int* InOrder_get(AVLNodePtr root, int* array);
int merge_arrays(int* thee_array, int* array1, int* array2, int thee_size, int size1, int size2);
AVLNodePtr creat_avl_tree(int start, int end, int* array);
int update_sum(AVLNodePtr root);
int update_size(AVLNodePtr root);

//search for a node with key <x>
AVLNodePtr avl_search(AVLNodePtr root,int x)
{
AVLNodePtr root_tmp=root;//make sure not to lose root.
while(root_tmp)
{
if(root_tmp->key==x) return root_tmp;//if current key=x, bingo.
//if x<current root -> we go left, else, we go right
    root_tmp=(root_tmp->key>x)?root_tmp->child[0]:root_tmp->child[1];//if x<value, go left, else, go right.
}
return root_tmp; // if reached here, we didn't find x key.
//the runtime here is O(h) since we check one path, h=logn since it's an avl tree.
}

AVLNodePtr avl_insert(AVLNodePtr root,int x) {

   if(!root){//if root is null, we creat a root
       AVLNode  *son = (AVLNode* ) malloc(sizeof(AVLNode));
       if(!son){return NULL;}
       creat_node(son,x);
       root=son;
       return root;
   }
   if(avl_search(root,x)){return root;}//if x is already in tree, we do nothing
   return update(root,x,1,x);
}

AVLNodePtr avl_delete(AVLNodePtr root,int x)
{
    AVLNodePtr tmp=avl_search(root,x);
    if(!tmp){return root;} //enters the if when null root or x isn't found.
    //case 1: x is a leaf
    if(!tmp->child[0]&&!tmp->child[1]){
      return update(root,x,-1,-x);
    }
    //case 2: x has one son
    //case 2.1: has a right son (y>x):
   if(!tmp->child[0]&&tmp->child[1]) {
       AVLNodePtr save=tmp->child[1];
       copy(tmp->child[1],tmp); //tmp=tmp->child[1]
       save->child[0]=NULL;
       save->child[1]=NULL;
       free(save); //free additional node
       root=update(root,tmp->key,0,-x);//this will update the sum and the height.
       return root;
   }
   //case 2.2: has a left son (x>y):
   if(!tmp->child[1]&&tmp->child[0]){
       AVLNodePtr save=tmp->child[0];
       copy(tmp->child[0],tmp); //tmp=tmp->child[0]
       save->child[0]=NULL;
       save->child[1]=NULL;
       free(save); //free additional node
       root=update(root,tmp->key,0,-x);//this will update the sum and height.
       return root;
   }
   //case 3: has 2 sons->switch x value with successor, delete x in successors place.
    {    //first we find successor:
        AVLNodePtr s;
        s=successor(root,x);
        int s_value=s->key;
        //we change the sum in x node
        tmp->tree_sum+=s_value-x;
        //now we "delete" x from it's path= minus the sum without changing the height of the path, and without actually deleting the node.
        root=update(root,x,0,s_value-x); //this will add 0 to height, -x to sum and won't delete the node.
        //now we delete successor: it's either a leaf or has a right son:
        {
            //case 1: x is a leaf
            if(!s->child[0]&&!s->child[1]){
                root=update(root,s_value,-1,-s_value);
            }
            //case 2: has a right son (y>x):
            if(!s->child[0]&&s->child[1]) {
                AVLNodePtr save=s->child[1];
                copy(s->child[1],s); //s=s->child[1]
                save->child[0]=NULL;
                save->child[1]=NULL;
                free(save); //free additional node
                root=update(root,s->key,0,-s_value);//this will update the sum and the height.
            }
        }
        //not forgetting to change the key of tmp at last
        tmp->key=s_value;
    }
   return root;
}

AVLNodePtr avl_merge(AVLNodePtr root1,AVLNodePtr root2)
{   if(!root1&&!root2){return NULL;}
    //firstly, we save the sorted values of each tree in arrays.
    int size1=0;
    if(root1){size1=root1->tree_size;}
    int array1[size1];
    InOrder_get(root1,array1); //O(n), array1 now holds the values of root1 in ascending order.
   // int size2=find_size(root2);
    int size2=0;
    if(root2){size2=root2->tree_size;}
    int array2[size2];
    InOrder_get(root2,array2); //O(n), array2 holds the values of root2.
    //now we merge into one big sorted array. (merge =O(n)).
    int array3[size1+size2];
    int size= merge_arrays(array3,array1,array2,size1+size2,size1,size2);
    AVLNodePtr root=creat_avl_tree(0, size-1, array3);
    return root;
}

AVLNodePtr avl_LCA(AVLNodePtr root,int x,int y)
{   if(!root)return root;
    //Compare the paths until the first different value
    AVLNodePtr tmp_x=root;
    AVLNodePtr tmp_y=root;
    AVLNodePtr save=root;
    while(tmp_x&&tmp_y){
        if(tmp_x!=tmp_y){break;}
        save=tmp_x;
        if(tmp_x->key==x){return tmp_x;}
        if(tmp_y->key==y){return tmp_x;}
        tmp_x=(tmp_x->key>x)?tmp_x->child[0]:tmp_x->child[1];
        tmp_y=(tmp_y->key>y)?tmp_y->child[0]:tmp_y->child[1];
    }
    if((save->key<x&&save->key<y)||(save->key>x&&save->key>y)){
        return NULL; //in case both x and y are greater than max, we return null. same for min..
    }
    return save;
}

int avl_sum(AVLNodePtr root,int x,int y)
{
    if(!root) {return 0;}  //just in case.
   //find lca, go along the path of each key, minus the sum of right son when going left.
   AVLNodePtr lca_tmp=avl_LCA(root,x,y);
    if(!lca_tmp)  {return 0;} //just in case.
    int sum=lca_tmp->tree_sum;
    //there are two cases: lca_tmp points to x/y, or x and y are on different sides
    //case 1:
   if(lca_tmp->key==x){
       if(x>y){
           sum=update_sum_left(lca_tmp, y, sum);
           sum-=right_sum(lca_tmp);
       }
       else   {
           sum=update_sum_right(lca_tmp, y, sum);
           sum-=left_sum(lca_tmp);
       }
   return sum;
   }
    if(lca_tmp->key==y){
        if(y>x){
            sum=update_sum_left(lca_tmp, x, sum);
            sum-=right_sum(lca_tmp);
        }
        else   {
            sum=update_sum_right(lca_tmp, x, sum);
            sum-=left_sum(lca_tmp);
        }
        return sum;
    }
   //case 2:
    AVLNodePtr tmp1=lca_tmp->child[0];
    AVLNodePtr tmp2=lca_tmp->child[1];
    sum=update_sum_left(tmp1, util_min(x, y), sum);//first path
    sum=update_sum_right(tmp2, util_max(x, y), sum);//second path
    return sum;
}

AVLNodePtr new_avl_node( int x )
{
    AVLNode  *son = (AVLNode* ) malloc(sizeof(AVLNode));
    if(!son){return NULL;}
    creat_node(son,x);
    return son;
}

void avl_delete_all (AVLNodePtr root)
{
    if(!root)return;
    avl_delete_all(root->child[0]);
    avl_delete_all(root->child[1]);
    free(root);
} //runtime = 0(n)

//additional private functions:

//creates a node with value x. doesn't return anything- no need.
void creat_node(AVLNodePtr son, int x){
son->child[0]=NULL;
son->child[1]=NULL;
son->key=x;
son->height=0;
son->tree_size=1;
son->tree_sum=x;
}

AVLNodePtr balance(AVLNodePtr root){
//there are 4 cases for legal rotations:
//check if balanced:
       if(!root) return root;
       int left_h=(root->child[0])?root->child[0]->height:-1;
       int right_h=(root->child[1])?root->child[1]->height:-1;
       if(util_abs(left_h-right_h)<=1){return root;}
        AVLNodePtr tmp_ptr;
    //case 1:
       if(left_h>right_h){
       tmp_ptr=root->child[0]; //tmp points to the left son.
        //we know for sure that tmp isn't null, we have to check if the same can be said for it's sons.
       int left_left_h=(tmp_ptr->child[0])?tmp_ptr->height:-1;
        int left_right_h=(tmp_ptr->child[1])?tmp_ptr->height:-1;
        if(left_left_h>=left_right_h){
            root=rotate_right(root, root->child[0]);
        }
    //case 2:(it's passed as case 3 in lectures but who's counting? we're looking for efficiency here)
        else
        {   //in this case, we have to rotate twice.
            root->child[0]=rotate_left(root->child[0], root->child[0]->child[1]); //after returning from this function, tmp will point to the updated left son of current root.
            root=rotate_right(root, root->child[0]);
        }
    }
    //case 3:(passed as case 2 in lectures)
       else
    {
        tmp_ptr=root->child[1]; //tmp points to the right son.
        //we know for sure that tmp isn't null, we have to check if the same can be said for it's sons.
        int right_left_h=(tmp_ptr->child[0])?tmp_ptr->height:-1;
        int right_right_h=(tmp_ptr->child[1])?tmp_ptr->height:-1;
        if(right_left_h<=right_right_h){
            root=rotate_left(root, root->child[1]);
        }
        //case 4:
        else
        {   //in this case, we have to rotate twice.
            root->child[1]=rotate_right(root->child[1], root->child[1]->child[0]); //after returning from this function, tmp will point to the updated right son of current root.
            root=rotate_left(root, root->child[1]);
        }
    }
    return root;
 }


AVLNodePtr rotate_right(AVLNodePtr root, AVLNodePtr tmp_ptr){
    return neutral_rotation(root, tmp_ptr, 0, 1);
 }

AVLNodePtr rotate_left(AVLNodePtr root, AVLNodePtr tmp_ptr){
   return neutral_rotation(root, tmp_ptr, 1, 0);
}

AVLNodePtr neutral_rotation(AVLNodePtr root, AVLNodePtr tmp_ptr, int i, int j){
    root->child[i]=tmp_ptr->child[j]; //root now points to the right son of it's left son.
    //update the sum of current root, since the tree has changed:
    root->tree_sum-=tmp_ptr->tree_sum;
    root->tree_size-=tmp_ptr->tree_size;//////////
    root->tree_sum+=(root->child[i])?root->child[i]->tree_sum:0;
    root->tree_size+=(root->child[i])?root->child[i]->tree_size:0;
    //update height of current root:
    int h1=(root->child[i])?root->child[i]->height:-1;
    int h2=(root->child[j])?root->child[j]->height:-1;
    root->height=util_max(h1,h2)+1;
    tmp_ptr->child[j]=root;//tmp is now the root.
    //update the sum and size of tmp:
    tmp_ptr->tree_sum+=root->tree_sum;
    tmp_ptr->tree_size+=root->tree_size;
    tmp_ptr->tree_sum-=(root->child[i])?root->child[i]->tree_sum:0;
    tmp_ptr->tree_size-=(root->child[i])?root->child[i]->tree_size:0;
    //update height:
    h1=(tmp_ptr->child[i])?tmp_ptr->child[i]->height:-1;
    h2=(tmp_ptr->child[j])?tmp_ptr->child[j]->height:-1;
    tmp_ptr->height=util_max(h1,h2)+1;

    return tmp_ptr;
    //after this rotation, we get a legal sub tree with an updated height and sum.
}

void copy(AVLNodePtr son, AVLNodePtr root){
     root->key=son->key;
     root->height=son->height;
     root->tree_sum=son->tree_sum;
     root->tree_size=son->tree_size;
     root->child[0]=son->child[0];
     root->child[1]=son->child[1];
 }

 //kind of successor...
AVLNodePtr successor(AVLNodePtr root, int x){
     AVLNodePtr tmp=avl_search(root,x);
     if(!tmp)return NULL;
     if(tmp->child[1]){ //if x.R!=null:
         tmp=tmp->child[1];
         while(tmp->child[0]){  //min(x.R)
             tmp=tmp->child[0];
         }
         return tmp;
     }
     //if x.R=null:
  /*   tmp=root;
     while(tmp)
     {
      }*/
     return tmp;
 }

int update_height(AVLNodePtr root, int x, int height_upd){
    if(!root){return -1;}//just in case
    int h1=(root->child[0])?root->child[0]->height:-1;
    int h2=(root->child[1])?root->child[1]->height:-1;
    return (1+util_max(h1,h2));
 }

int update_sum(AVLNodePtr root){
    if(!root){return 0;}//just in case
    int s1=(root->child[0])?root->child[0]->tree_sum:0;
    int s2=(root->child[1])?root->child[1]->tree_sum:0;
    return (root->key+(s1+s2));
}

int update_size(AVLNodePtr root){
    if(!root){return 0;}//just in case
    int s1=(root->child[0])?root->child[0]->tree_size:0;
    int s2=(root->child[1])?root->child[1]->tree_size:0;
    return (1+(s1+s2));
}


void update_nodes(AVLNodePtr root, int x, int height_upd, int sum){
    if(!root){return;}
    AVLNodePtr root_tmp;//because we're using recursion.
    root_tmp=(root->key>x)?root->child[0]:root->child[1];//if x<value, go left, else, go right.

    if(!root_tmp){ //son is null - x should be here.
        AVLNode * son = (AVLNode *) malloc(sizeof(AVLNode));
        if(!son){return;}
        creat_node(son,x);
        if (root->key > x) { root->child[0] = son; }
        else { root->child[1] = son; }
        return;
    }
    //if reached here, son isn't null
    if(root_tmp->key==x) {
        if (height_upd < 0) { //if height=-1,delete.
            free(root_tmp);//after we free, we have to make sure that son is null and not a garbage values
            if(root->key>x){root->child[0]=NULL;}
            else {root->child[1]=NULL;}
            return;
        }
       return;
    }
   update_nodes(root_tmp,x,height_upd,sum);//recursion till we get to x.
    //now we update:
    root_tmp->height=update_height(root_tmp, x, height_upd);
    root_tmp->tree_sum+=sum;
    root_tmp->tree_size=update_size(root_tmp);
    //now we balance:
    AVLNodePtr save= balance(root_tmp);
    if(root->key>x){root->child[0]=save;}
    else {root->child[1]=save;}
}

//our main function is the one above. this function creates a "root" so our root becomes a right son, because the update_node function works with sons only.
AVLNodePtr update(AVLNodePtr root, int x, int height_upd, int sum){
    if(!root)return root;
    AVLNode * tmp_root = (AVLNode *) malloc(sizeof(AVLNode));
    if(!tmp_root){return NULL;}
    creat_node(tmp_root,util_min(x-1,root->key)-1);
    tmp_root->child[1]=root;
    update_nodes(tmp_root,x,height_upd,sum);
    root=tmp_root->child[1];
    free(tmp_root);
    return root;
}

int right_sum(AVLNodePtr lca_tmp){
    if(!lca_tmp){return 0;}
    if(!lca_tmp->child[1]) {return 0;}
    return (lca_tmp->child[1]->tree_sum);
}

int left_sum(AVLNodePtr lca_tmp){
    if(!lca_tmp){return 0;}
    if(!lca_tmp->child[0]) {return 0;}
    return (lca_tmp->child[0]->tree_sum);
}

int update_sum_right(AVLNodePtr lca_tmp, int s, int sum){
    while((lca_tmp)&&lca_tmp->key!=s){
        if(lca_tmp->key>s){
            sum-=right_sum(lca_tmp);
            sum-=lca_tmp->key;
            lca_tmp=lca_tmp->child[0];
        }
        else {lca_tmp=lca_tmp->child[1];}
    }
   sum-=right_sum(lca_tmp);
   return sum;
}

int update_sum_left(AVLNodePtr lca_tmp, int s, int sum){
    while((lca_tmp)&&lca_tmp->key!=s){
        if(lca_tmp->key<s){
            sum-=left_sum(lca_tmp);
            sum-=lca_tmp->key;
            lca_tmp=lca_tmp->child[1];
        }
        else {lca_tmp=lca_tmp->child[0];}
    }
    sum-=left_sum(lca_tmp);
    return sum;
}


int* InOrder_get(AVLNodePtr root, int* array){
    if(!root) return array;
    array=InOrder_get(root->child[0], array);
    *array=root->key;
    array++;
    array=InOrder_get(root->child[1], array);
    return array;
}

int merge_arrays(int* thee_array, int* array1, int* array2, int thee_size, int size1, int size2){
    int i=0;
    int j=0;
    int counter=0;
    if(!size1&&!size2){return 0;}
    for(int k=0; k<thee_size; k++){

        if(j==size2&&i==size1)break;
        counter++;
        if(i==size1){//reached the end of array1
            thee_array[k]=array2[j];
            j++;
        }
        else if(j==size2){//reached the end of array2
            thee_array[k]=array1[i];
            i++;
        }
        else {
            if(array1[i]==array2[j]){
                thee_array[k]=array1[i];
                i++;
                j++;
            }
            else if(array1[i]<array2[j]){
                thee_array[k]=array1[i];
                i++;
            }
            else {
                thee_array[k]=array2[j];
                j++;
            }
        }
    }
    return counter;
}

AVLNodePtr creat_avl_tree(int start, int end, int* array){
    if(start>end||end<start){return NULL;}
    if(start==end){
        AVLNode  *son = (AVLNode* ) malloc(sizeof(AVLNode));
        if(!son){return NULL;}
        creat_node(son,array[start]);
        return son;
    }
    AVLNode  *son = (AVLNode* ) malloc(sizeof(AVLNode));
    if(!son){return NULL;}
    creat_node(son,array[start+(end-start)/2]); //each sub-tree root has the middle value of array.
    son->child[0]=creat_avl_tree(start,start+((end-start)/2)-1, array);
    son->child[1]=creat_avl_tree(start+((end-start)/2)+1,end, array);
    //first, we update height:
    son->height=update_height(son, 0, 0);
    son->tree_sum=update_sum(son);
    son->tree_size=update_size(son);
    return son;
}
