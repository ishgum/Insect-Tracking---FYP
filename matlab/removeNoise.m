function [ array ] = removeNoise(array)
% Emulates method used on UAV, returns array with non pulse samples 
%   set to NaN.

% simple threshold
% average = mean(array);
% array = array-mean;
% abs(array)
inPulse = 0;
pulseSum = 0;
for N=1:length(array)
    if array(N) < 0.7
        array(N) = NaN;
        if inPulse
            array(N) = pulseSum/inPulse;
            pulseSum = 0;
            inPulse = 0;
        end
    else
        inPulse = inPulse + 1;
        pulseSum = pulseSum + array(N);
        array(N) = NaN;
    end
end
end

