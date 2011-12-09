
filename = 'testInput1.in'
N = 100; % Number of bodies

POS_MAX = 10; % +- max value, centered about zero
VEL_MAX = 0.5; % +- max value, centered about zero

p = (rand(N,3)-0.5) * POS_MAX;
v = (rand(N,3)-0.5) * VEL_MAX;

data = [p v];

f = fopen(filename,'w');

fprintf(f,'%i\n',N);

fclose(f);

dlmwrite(filename,data,'-append','delimiter',' ');
