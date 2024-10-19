#include "binaryTree.h"


// typedef char BTDataType;

// typedef struct BinaryTreeNode
// {
// 	BTDataType _data;
// 	struct BinaryTreeNode* _left;
// 	struct BinaryTreeNode* _right;
// }BTNode;

// 通过前序遍历的数组"ABD##E#H##CF##G##"构建二叉树
BTNode* BinaryTreeCreate(BTDataType* a, int n, int* pi) {
    if (n == *pi)
        return NULL;
    if (a[*pi] == '#' && (*pi)++)
        return NULL;
    BTNode* node = (BTNode*)malloc(sizeof(BTNode));
    node->_data = a[(*pi)++];
    node->_left = BinaryTreeCreate(a,n,pi);
    node->_right = BinaryTreeCreate(a,n,pi);
    return node;
}
// 二叉树销毁
void BinaryTreeDestory(BTNode** root) {
    if (!(*root))
        return;
    BinaryTreeDestory(&(*root)->_left);
    BinaryTreeDestory(&(*root)->_right);
    free(*root);
    *root = NULL;
}



// 二叉树节点个数
int BinaryTreeSize(BTNode* root) {
    if (!root)
        return 0;
    return BinaryTreeSize(root->_left) + BinaryTreeSize(root->_right) + 1;
}
// 二叉树叶子节点个数
int BinaryTreeLeafSize(BTNode* root) {
    if (!root)
        return 0;
    if (!root->_left && !root->_right)
        return 1;
    return BinaryTreeLeafSize(root->_left) + BinaryTreeLeafSize(root->_right);
}
// 二叉树第k层节点个数
int BinaryTreeLevelKSize(BTNode* root, int k) {
    if (!root || k < 1)
        return 0;
    if (!(k - 1))
        return 1;
    return BinaryTreeLevelKSize(root->_left, k - 1) + BinaryTreeLevelKSize(root->_right, k - 1);
}
// 二叉树查找值为x的节点
BTNode* BinaryTreeFind(BTNode* root, BTDataType x) {
    if (!root)
        return NULL;
    if (root->_data == x)
        return root;
    BTNode* node = BinaryTreeFind(root->_left,x);
    if (node)
        return node;
    return BinaryTreeFind(root->_right, x);
}

void order(BTNode* root, int i) {
    if (!root)
        return;
    if (i == 1)//前序
        printf("%c ",root->_data);
    order(root->_left, i);
    if (i == 2)//中序
        printf("%c ",root->_data);
    order(root->_right, i);
    if (i == 3)//后续
        printf("%c ",root->_data);
}

// 二叉树前序遍历 
void BinaryTreePrevOrder(BTNode* root) {
    order(root,1);
    printf("\n");
}
// 二叉树中序遍历
void BinaryTreeInOrder(BTNode* root){
    order(root,2);
    printf("\n");
}
// 二叉树后序遍历
void BinaryTreePostOrder(BTNode* root) {
    order(root,3);
    printf("\n");
}
// 层序遍历
void BinaryTreeLevelOrder(BTNode* root) {
    queue* q = init_queue(10);
    q_push(q,root);
    while (!q_empty(q)) {
        BTNode* node = q_front(q);
        q_pop(q);
        
        // if (!node)
        //     printf("node是空\n");
        // else 
        //     printf("%c被弹出\n",node->_data);
        printf("%c ",node->_data);
        if (node->_left){
            q_push(q,node->_left);
            //printf("%c被压入\n",node->_left->_data);
        }
        if (node->_right){
            q_push(q,node->_right);
            //printf("%c被压入\n",node->_right->_data);
        }
    }
    printf("\n");
    q_destory(&q);
}

int BinaryTreeHeight(BTNode* root) {
    if (!root)
        return 0;
    if (!root->_left && !root->_right)
        return 1;
    int left = BinaryTreeHeight(root->_left);
    int right = BinaryTreeHeight(root->_right);
    return (left > right ? left : right) + 1;
}

void push_node_queue(BTNode* root, int k, queue* q) {
    if (!root || k < 0)
        return;
    if (k - 1 == 0){
        q_push(q,root);
        return;
    }
    push_node_queue(root->_left, k - 1, q);
    push_node_queue(root->_right, k - 1, q);
}

int is_full(BTNode* root, int k) {
    if (!root || k < 2){
        return 0;
    }
    k--;
    queue* q = init_queue(20);
    push_node_queue(root,k,q);//将第k层的节点放入队列
    int flag = 0;
    while(!q_empty(q)){
        BTNode* node = q_front(q);
        q_pop(q);
        if (node->_left){
            if (flag){
                q_destory(&q);
                return 0;
            }
        }
        else {
            if (!flag)
                flag = 1;
        }
        if (node->_right){
            if (flag){
                q_destory(&q);
                return 0;
            }
        }
        else {
            if (!flag)
                flag = 1;
        }
    }
    return 1;
}

// 判断二叉树是否是完全二叉树
int BinaryTreeComplete1(BTNode* root) {
    if(!root)
        return 1;
    int height = BinaryTreeHeight(root);
    for (int i = 1; i <= height; i++) {
        int k = BinaryTreeLevelKSize(root, i);
        int k_size = (int)pow(2.0,i - 1 + 0.0);
        if (i < height && k < k_size)
            return 0;
        if (i == height){
            if (k == k_size){
                return 1;
            }
            else {
                return is_full(root,height);
            }
        }
    }
}
int BinaryTreeComplete2(BTNode* root) {
    if(!root)
        return 1;
    int flag = 0;
    queue* q = init_queue(20);
    q_push(q,root);
    while(!q_empty(q)) {
        BTNode* node = q_front(q);
        q_pop(q);
        if (node->_left){
            if (flag){
                q_destory(&q);
                return 0;
            }
            q_push(q,node->_left);
        }
        else{
            flag = 1;
        }
        if (node->_right){
            if (flag){
                q_destory(&q);
                return 0;
            }
            q_push(q,node->_right);
        }
        else{
            flag = 1;
        }
    }
    q_destory(&q);
    return 1;
}