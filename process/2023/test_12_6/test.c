

#include "binaryTree.h"
#include <string.h>

int main() {
    //char str[] = "AB##c##";
    char str[] = "ABD##E#H##CF##G##";
    int len = strlen(str);
    int pi = 0;
    //创建二叉树
    BTNode* root = BinaryTreeCreate(str,len,&pi);
    //前序遍历
    BinaryTreePrevOrder(root);
    //中序
    BinaryTreeInOrder(root);
    //后序
    BinaryTreePostOrder(root);

    printf("节点个数：%d\n",BinaryTreeSize(root));
    printf("叶子结点个数：%d\n",BinaryTreeLeafSize(root));
    for (int i = 1;;i++){
        int count = BinaryTreeLevelKSize(root,i);
        if (count != 0)
            printf("第%d层有%d个节点",i,count);
        else
            break;
        printf("\n");
    }
    for (char i = 65; i < 90; i++){
        BTNode* node = BinaryTreeFind(root,i);
        if (node)
            printf("查找值为 %c ...找到了：%c\n",i,node->_data);
        else 
            printf("查找值为 %c ...没找到！\n");
    }

    //层序遍历
    BinaryTreeLevelOrder(root);

    //是否是完全二叉树
    int tmp = BinaryTreeComplete2(root);
    if(tmp == 1)
        printf("是完全二叉树\n");
    else
        printf("不是完全二叉树\n");

    //销毁二叉树
    BinaryTreeDestory(&root);
    return 0;
}