function [ pointArray ] = removeNoisePoints(array)
% Emulates method used on UAV
 %  simple threshold
% average = mean(array);
% array = array-mean;
% abs(array)
inPulse = 0;
pulseSum = 0;
pointArray = [];
pulseNum = 0;
for N=1:length(array)
    if array(N,2) < 0.6
        if inPulse
            pulseNum = pulseNum +1;
            pointArray(pulseNum,1) = array(N,1);
            pointArray(pulseNum,2) = pulseSum/inPulse;
            pulseSum = 0;
            inPulse = 0;
        end
    else
        inPulse = inPulse + 1;
        pulseSum = pulseSum + array(N,2);
    end
end
end

