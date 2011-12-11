N = input('Enter number of bodies: '); % Number of bodies
filename = sprintf('i%i.in',N);


POS_MAX = 10; % +- max value, centered about zero
VEL_MAX = .5; % +- max value, centered about zero

%p = (rand(N,3)-0.5) * POS_MAX;
%v = (rand(N,3)-0.5) * VEL_MAX;
p = randn(N,3) * POS_MAX;
v = randn(N,3) * VEL_MAX;

% % Normalize position to zero overall
% k = sum(p)/2;
% p = p - k(ones(size(p,1),1),:);
% 
% % Normalize velocity to zero overall
% k = sum(v)/2;
% v = v - k(ones(size(v,1),1),:);

data = [p v];

f = fopen(filename,'w');

fprintf(f,'%i\n',N);

fclose(f);

dlmwrite(filename,data,'-append','delimiter',' ');
fprintf('Wrote %i bodies to ''%s''\n',N,filename);