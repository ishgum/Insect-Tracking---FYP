function [ c ] = maf( a, num )
% %MAF, Circular convolution
filter = 1/num*ones(num,1);
c_padded = conv(repmat(a,3,1), filter);
c = c_padded(length(a)+1+round(num/2):2*length(a)+round(num/2));
end

