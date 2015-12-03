function [ pointArray, index ] = removeNoisePointsWithIndex(array)
% Emulates method used on UAV, returns smaller array with only the pulse samples 
 %  simple threshold
% average = mean(array);
% array = array-mean;
% abs(array)
inPulse = 0;
pulseSum = 0;
pointArray = [];
pulseNum = 0;
index = [];
index_i = 1;
for N=1:length(array)
    if array(N,2) < 0.6
        if inPulse
            pulseNum = pulseNum +1;
            pointArray(pulseNum,1) = array(N,1);
            pointArray(pulseNum,2) = pulseSum/inPulse;
            index(index_i) = N;
            index_i = index_i+ 1;
            pulseSum = 0;
            inPulse = 0;
        end
    else
        inPulse = inPulse + 1;
        pulseSum = pulseSum + array(N,2);
    end
end
end

