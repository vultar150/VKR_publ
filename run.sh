#!/bin/bash
for (( i = 1; i <= 3; i++ ))
do
./VKR_publ newData/dataFAR/20tasks/input$i\_far20.xml 1
echo "dataFAR 20 input$i"
echo "\n"
./VKR_publ newData/dataFAR/38tasks/input$i\_far38.xml 1
echo "dataFAR 38 input$i"
echo "\n"
./VKR_publ newData/dataFAR/56tasks/input$i\_far56.xml 1
echo "dataFAR 56 input$i"
echo "\n"
./VKR_publ newData/dataFAR/101tasks/input$i\_far101.xml 1
echo "dataFAR 101 input$i"
echo "\n"
done

for (( i = 1; i <= 3; i++ ))
do
./VKR_publ newData/dataKBO/5tasks/input$i\_kbo5.xml 1
echo "dataKBO 5 input$i"
echo "\n"
./VKR_publ newData/dataKBO/20tasks/input$i\_kbo20.xml 1
echo "dataKBO 20 input$i"
echo "\n"
./VKR_publ newData/dataKBO/38tasks/input$i\_kbo38.xml 1
echo "dataKBO 38 input$i"
echo "\n"
./VKR_publ newData/dataKBO/56tasks/input$i\_kbo56.xml 1
echo "dataKBO 56 input$i"
echo "\n"
./VKR_publ newData/dataKBO/101tasks/input$i\_kbo101.xml 1
echo "dataKBO 101 input$i"
echo "\n"
done

for (( i = 1; i <= 3; i++ ))
do
./VKR_publ newData/dataTree/5tasks/input$i\_tree5.xml 1
echo "dataTree 5 input$i"
echo "\n"
./VKR_publ newData/dataTree/20tasks/input$i\_tree20.xml 1
echo "dataTree 20 input$i"
echo "\n"
./VKR_publ newData/dataTree/38tasks/input$i\_tree38.xml 1
echo "dataTree 38 input$i"
echo "\n"
./VKR_publ newData/dataTree/56tasks/input$i\_tree56.xml 1
echo "dataTree 56 input$i"
echo "\n"
./VKR_publ newData/dataTree/101tasks/input$i\_tree101.xml 1
echo "dataTree 101 input$i"
echo "\n"
done