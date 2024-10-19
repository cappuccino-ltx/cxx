
create database searcher;
use searcher;
drop table if exists inverted_list;
drop table if exists forward_index;
drop table if exists inverted_index;


create table if not exists forward_index(
    doc_id int primary key auto_increment comment '文档的id，从1开始自增',
    title text not null comment '文档的标题',
    content mediumtext not null comment '文档对应的去标签之后的内容',
    url text not null comment '官网文档url'
)engine=innodb default charset=utf8mb4;

create table if not exists inverted_index(
    inv_id int primary key auto_increment comment '倒排拉链的id，从1开始自增',
    word text not null comment '倒排的词'
)engine=innodb default charset=utf8mb4;

create table if not exists inverted_list(
    list_id int primary key auto_increment comment '拉链的id，从1开始自增',
    inv_id int comment '外键到倒排索引的id字段',
    doc_id int comment '外键到正排索引的id字段',
    weight int not null comment '权值',
    foreign key (inv_id) references inverted_index(inv_id),
    foreign key (doc_id) references forward_index(doc_id)
)engine=innodb default charset=utf8mb4;