#!/usr/bin/sh
cat /dev/null > info          #清空info
types=("*.asm" "*.c" "*.h" )    #所有要统计的文件类型
for i in ${types[@]}          #遍历每个文件类型
do
    find . -iname $i > tmp    #按类型（大小写不敏感）查找，缓存到tmp
    cat tmp >> info           #将查询结果追加到info
done
sed -i 's/ /\\&/g' info       #处理文件名出现空格的情况
cat info | xargs wc -l        #统计行数
rm info tmp                   #删除临时文件
