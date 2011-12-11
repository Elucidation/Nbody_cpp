N = 100; % Number of bodies
filename = sprintf('i%i.in',N);


POS_MAX = 1; % +- max value, centered about zero
VEL_MAX = .5; % +- max value, centered about zero

p = (rand(N,3)-0.5) * POS_MAX;
v = (rand(N,3)-0.5) * VEL_MAX;

data = [p v];

f = fopen(filename,'w');

fprintf(f,'%i\n',N);

fclose(f);

dlmwrite(filename,data,'-append','delimiter',' ');
fprintf('Wrote %i bodies to ''%s''\n',N,filename);