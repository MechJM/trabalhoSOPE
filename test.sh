(./simpledu "$@" || echo $?) | sort -k2 > teste_simpledu.txt
(du -l "$@" || echo $?) | sort -k2 > teste_du.txt
diff -q teste_simpledu.txt teste_du.txt > /dev/null 2>&1 && echo OK || echo FAILED
