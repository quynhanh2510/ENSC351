#!/bin/bash
> AllMarks.txt
git_dir="/media/sf_ENSC351/git/ensc351lib/Ensc351/"
#git_dir="/Users/yuhuig/Documents/School/Fall3031/ENSC351/git/ensc351lib/Ensc351"
LDFLAGS="-Wl,-rpath,/usr/lib/x86_64-linux-gnu -L/usr/lib/x86_64-linux-gnu -Wl,-rpath,$git_dir/Debug -L$git_dir/Debug -lboost_system -lboost_thread -lboost_unit_test_framework -lpthread"
pp3_dir="pp3"

##compile and link first for ease of double checking
compile() {
	local sub_dir=$1
	echo $sub_dir
	cp Ensc351Part3.cpp $sub_dir/Ensc351Part3.cpp
	#cp -n  myIO.cpp $sub_dir/myIO.cpp
	cp -n  myIO.h $sub_dir/myIO.h
	cp Medium.cpp $sub_dir/Medium.cpp
	cp Medium.h $sub_dir/Medium.h	
	cp SenderY.h $sub_dir/SenderY.h	
	cp SenderY.cpp $sub_dir/SenderY.cpp	
	#cp SenderSS.h $sub_dir/SenderSS.h	
	#cp SenderSS.cpp $sub_dir/SenderSS.cpp	
	cp ReceiverY.h $sub_dir/ReceiverY.h	 
	cp ReceiverY.cpp $sub_dir/ReceiverY.cpp	 
	cp PeerY.cpp $sub_dir/PeerY.cpp
	cp PeerY.h $sub_dir/PeerY.h

	g++ -std=c++17 -I$git_dir -I"/usr/include"  -O0 -g3 -c -Wno-register -Wno-return-type -fmessage-length=0 -MMD -MP -MF "$sub_dir/Ensc351Part3.d" -MT "$sub_dir/Ensc351Part3.o" -o $sub_dir/Ensc351Part3.o $sub_dir/Ensc351Part3.cpp &&
	g++ -std=c++17 -I$git_dir -I"/usr/include"  -O0 -g3 -c -Wno-register -Wno-return-type -fmessage-length=0 -MMD -MP -MF "$sub_dir/SenderY.d" -MT "$sub_dir/SenderY.o" -o "$sub_dir/SenderY.o" "$sub_dir/SenderY.cpp" &&
	g++ -std=c++17 -I$git_dir -I"/usr/include"  -O0 -g3 -c -Wno-register -Wno-return-type -fmessage-length=0 -MMD -MP -MF "$sub_dir/Medium.d" -MT "$sub_dir/Medium.o" -o "$sub_dir/Medium.o" "$sub_dir/Medium.cpp" &&
	g++ -std=c++17 -I$git_dir -I"/usr/include"  -O0 -g3 -c -Wno-register -Wno-return-type -fmessage-length=0 -MMD -MP -MF "$sub_dir/PeerY.d" -MT "$sub_dir/PeerY.o" -o "$sub_dir/PeerY.o" "$sub_dir/PeerY.cpp" &&
	g++ -std=c++17 -I$git_dir -I"/usr/include"  -O0 -g3 -c -Wno-register -Wno-return-type -fmessage-length=0 -MMD -MP -MF "$sub_dir/ReceiverY.d" -MT "$sub_dir/ReceiverY.o" -o "$sub_dir/ReceiverY.o" "$sub_dir/ReceiverY.cpp" &&
	g++ -std=c++17 -I$git_dir -I"/usr/include"  -O0 -g3 -c -Wno-register -Wno-return-type -fmessage-length=0 -MMD -MP -MF "$sub_dir/myIO.d" -MT "$sub_dir/myIO.o" -o "$sub_dir/myIO.o" "$sub_dir/myIO.cpp" &&
	g++ $LDFLAGS -L$git_dir/Debug -o "$sub_dir/Ensc351Part3"  $sub_dir/Ensc351Part3.o $sub_dir/Medium.o $sub_dir/PeerY.o $sub_dir/myIO.o $sub_dir/ReceiverY.o $sub_dir/SenderY.o -lEnsc351 -lboost_system -lboost_thread -l"boost_unit_test_framework" -lpthread
}

for i in $(ls -d $pp3_dir/*)
do
	compile "$i" 
	continue
done
#wait

for i in $(ls -d $pp3_dir/*)
do
	echo $i
	#delete previous binary
	rm Ensc351Part3
	#copy from pp3
	\cp $i/Ensc351Part3 ./Ensc351Part3

	#if binary doesnt exist (compile error), continue
	if [ ! -f Ensc351Part3 ]; then
		echo "compile_error\n" >> $i/error.txt
		continue
	fi

	timeout 1m ./Ensc351Part3 --log_level=all --run_test=priority_test
	mv priority-test-score.txt $i/
	#for j in 0 1 2 3 4 5 6
	#do
		#curr_test="test${j}"
		##timeout -k 1m 50s 
		#timeout 1m ./Ensc351Part3 --log_level=all --run_test=$curr_test
		#mv ymodemData.dat $i/"input$j.dat"
		##\cp "input_$1.txt" $i/"transferredFile_"$curr_test
	#done
done
