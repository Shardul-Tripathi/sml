uint32_t w[10][785];
uint32_t x[785];
uint32_t ans[10];

for uint32_t i = [0:10] {
    for uint32_t j = [0:785] {
        w[i][j] = input1(50);
    }
}

for uint32_t i = [0:785] {
    x[i] = input2(100);
}

uint32_t tmp;
for uint32_t i = [0:10] {
    tmp = 0;
    for uint32_t j = [0:785] {
        tmp = tmp + w[i][j]*x[j];
    }
    ans[i] = tmp;
}

uint32_t max;
max = 0;
for uint32_t i = [0:10] {
    max = (ans[i] > max) ? ans[i] : max;
}

output(max);