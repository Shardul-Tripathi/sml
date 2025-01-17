ABYParty *party = new ABYParty(role, address, port, seclvl, bitlen, nthreads, mt_alg, 100000000);
vector<Sharing*>& sharings = party->GetSharings();
Circuit* ycirc = sharings[S_YAO]->GetCircuitBuildRoutine();
Circuit* acirc = sharings[S_ARITH]->GetCircuitBuildRoutine();
Circuit* bcirc = sharings[S_BOOL]->GetCircuitBuildRoutine();
auto x = make_vector<uint32_t>(33, 33);
auto s_a_x = make_vector<share*>(33, 33);
auto wconv1 = make_vector<uint32_t>(32, 5, 5);
auto s_a_wconv1 = make_vector<share*>(32, 5, 5);
auto bconv1 = make_vector<uint32_t>(32);
auto s_a_bconv1 = make_vector<share*>(32);
auto hconv1 = make_vector<uint32_t>(32, 28, 28);
auto s_a_hconv1 = make_vector<share*>(32, 28, 28);
for (uint32_t i = 0; i < 33; i++)
{
    for (uint32_t j = 0; j < 33; j++)
    {
        if (role == SERVER) {
            x[i][j] = 450 ;
            s_a_x[i][j] = acirc->PutINGate( x[i][j] ,bitlen,SERVER);
        } else {
            s_a_x[i][j] = acirc->PutDummyINGate(bitlen);
        }
    }
}
for (uint32_t i = 0; i < 32; i++)
{
    for (uint32_t j = 0; j < 5; j++)
    {
        for (uint32_t k = 0; k < 5; k++)
        {
            if (role == CLIENT) {
                wconv1[i][j][k] = 500 ;
                s_a_wconv1[i][j][k] = acirc->PutINGate( wconv1[i][j][k] ,bitlen,CLIENT);
            } else {
                s_a_wconv1[i][j][k] = acirc->PutDummyINGate(bitlen);
            }
        }
    }
}
for (uint32_t i = 0; i < 32; i++)
{
    if (role == CLIENT) {
        bconv1[i] = 66882 ;
        s_a_bconv1[i] = acirc->PutINGate( bconv1[i] ,bitlen,CLIENT);
    } else {
        s_a_bconv1[i] = acirc->PutDummyINGate(bitlen);
    }
}
for (uint32_t i = 0; i < 32; i++)
{
    for (uint32_t j = 0; j < 28; j++)
    {
        for (uint32_t k = 0; k < 28; k++)
        {
            hconv1[i][j][k] = 0 ;
            s_a_hconv1[i][j][k] = acirc->PutCONSGate( hconv1[i][j][k] ,bitlen);
            for (uint32_t l = 0; l < 5; l++)
            {
                for (uint32_t m = 0; m < 5; m++)
                {
                    share * s_a_tmp_0 = acirc->PutMULGate( s_a_wconv1[i][l][m] , s_a_x[j+l][k+m] );
                    s_a_hconv1[i][j][k] = acirc->PutADDGate( s_a_hconv1[i][j][k] , s_a_tmp_0 );
                }
            }
            s_a_hconv1[i][j][k] = acirc->PutADDGate( s_a_hconv1[i][j][k] , s_a_bconv1[i] );
        }
    }
}
auto s_y_hconv1 = make_vector<share*>(32, 28, 28);
for (int _i1 = 0; _i1 < 32 ; _i1++) {
for (int _i4 = 0; _i4 < 28 ; _i4++) {
for (int _i7 = 0; _i7 < 28 ; _i7++) {
s_y_hconv1[_i1][_i4][_i7] = ycirc->PutA2YGate( s_a_hconv1[_i1][_i4][_i7] );
}
}
}
for (uint32_t i = 0; i < 32; i++)
{
    for (uint32_t j = 0; j < 28; j++)
    {
        for (uint32_t k = 0; k < 28; k++)
        {
            uint32_t _tmp_2 = 2147483648 ;
            share * s_y__tmp_2 = ycirc->PutCONSGate( _tmp_2 ,bitlen);
            share * s_y_tmp_1 = ycirc->PutGTGate( s_y_hconv1[i][j][k] , s_y__tmp_2 );
            uint32_t _tmp_3 = 0 ;
            share * s_y__tmp_3 = ycirc->PutCONSGate( _tmp_3 ,bitlen);
            vector<uint32_t> _vec5 = s_y_hconv1[i][j][k]->get_wires();
            _vec5.erase( _vec5.begin(), _vec5.begin() + 8 );
            share *s_y_tmp_4 = create_new_share( _vec5 , ycirc );
            s_y_hconv1[i][j][k] = ycirc->PutMUXGate( s_y__tmp_3 , s_y_tmp_4 , s_y_tmp_1 );
        }
    }
}
auto hpool1 = make_vector<uint32_t>(32, 19, 19);
auto s_y_hpool1 = make_vector<share*>(32, 19, 19);
for (uint32_t i = 0; i < 32; i++)
{
    for (uint32_t j = 0; j < 28; j += 2)
    {
        for (uint32_t k = 0; k < 28; k += 2)
        {
            hpool1[i][j/2][k/2] = 0 ;
            s_y_hpool1[i][j/2][k/2] = ycirc->PutCONSGate( hpool1[i][j/2][k/2] ,bitlen);
            share * s_y_tmp_6 = ycirc->PutGTGate( s_y_hpool1[i][j/2][k/2] , s_y_hconv1[i][j][k] );
            s_y_hpool1[i][j/2][k/2] = ycirc->PutMUXGate( s_y_hpool1[i][j/2][k/2] , s_y_hconv1[i][j][k] , s_y_tmp_6 );
            share * s_y_tmp_7 = ycirc->PutGTGate( s_y_hpool1[i][j/2][k/2] , s_y_hconv1[i][j+1][k] );
            s_y_hpool1[i][j/2][k/2] = ycirc->PutMUXGate( s_y_hpool1[i][j/2][k/2] , s_y_hconv1[i][j+1][k] , s_y_tmp_7 );
            share * s_y_tmp_8 = ycirc->PutGTGate( s_y_hpool1[i][j/2][k/2] , s_y_hconv1[i][j][k+1] );
            s_y_hpool1[i][j/2][k/2] = ycirc->PutMUXGate( s_y_hpool1[i][j/2][k/2] , s_y_hconv1[i][j][k+1] , s_y_tmp_8 );
            share * s_y_tmp_9 = ycirc->PutGTGate( s_y_hpool1[i][j/2][k/2] , s_y_hconv1[i][j+1][k+1] );
            s_y_hpool1[i][j/2][k/2] = ycirc->PutMUXGate( s_y_hpool1[i][j/2][k/2] , s_y_hconv1[i][j+1][k+1] , s_y_tmp_9 );
        }
    }
    for (uint32_t j = 14; j < 19; j++)
    {
        for (uint32_t k = 0; k < 19; k++)
        {
            hpool1[i][j][k] = 0 ;
            s_y_hpool1[i][j][k] = ycirc->PutCONSGate( hpool1[i][j][k] ,bitlen);
        }
    }
    for (uint32_t j = 0; j < 19; j++)
    {
        for (uint32_t k = 14; k < 19; k++)
        {
            hpool1[i][j][k] = 0 ;
            s_y_hpool1[i][j][k] = ycirc->PutCONSGate( hpool1[i][j][k] ,bitlen);
        }
    }
}
auto wconv2 = make_vector<uint32_t>(64, 5, 5);
auto s_y_wconv2 = make_vector<share*>(64, 5, 5);
auto bconv2 = make_vector<uint32_t>(64);
auto s_y_bconv2 = make_vector<share*>(64);
auto hconv2 = make_vector<uint32_t>(64, 14, 14);
auto s_y_hconv2 = make_vector<share*>(64, 14, 14);
auto s_a_wconv2 = make_vector<share*>(64, 5, 5);
for (uint32_t i = 0; i < 64; i++)
{
    for (uint32_t j = 0; j < 5; j++)
    {
        for (uint32_t k = 0; k < 5; k++)
        {
            if (role == CLIENT) {
                wconv2[i][j][k] = 500 ;
                s_a_wconv2[i][j][k] = acirc->PutINGate( wconv2[i][j][k] ,bitlen,CLIENT);
            } else {
                s_a_wconv2[i][j][k] = acirc->PutDummyINGate(bitlen);
            }
        }
    }
}
auto s_a_bconv2 = make_vector<share*>(64);
for (uint32_t i = 0; i < 64; i++)
{
    if (role == CLIENT) {
        bconv2[i] = 66882 ;
        s_a_bconv2[i] = acirc->PutINGate( bconv2[i] ,bitlen,CLIENT);
    } else {
        s_a_bconv2[i] = acirc->PutDummyINGate(bitlen);
    }
}
auto s_a_hconv2 = make_vector<share*>(64, 14, 14);
auto s_a_hpool1 = make_vector<share*>(32, 19, 19);
for (int _i1 = 0; _i1 < 32 ; _i1++) {
for (int _i4 = 0; _i4 < 19 ; _i4++) {
for (int _i7 = 0; _i7 < 19 ; _i7++) {
s_a_hpool1[_i1][_i4][_i7] = acirc->PutY2AGate( s_y_hpool1[_i1][_i4][_i7] ,bcirc);
}
}
}
for (uint32_t i = 0; i < 32; i++)
{
    for (uint32_t j = 0; j < 14; j++)
    {
        for (uint32_t k = 0; k < 14; k++)
        {
            hconv2[2*i][j][k] = 0 ;
            s_a_hconv2[2*i][j][k] = acirc->PutCONSGate( hconv2[2*i][j][k] ,bitlen);
            hconv2[2*i+1][j][k] = 0 ;
            s_a_hconv2[2*i+1][j][k] = acirc->PutCONSGate( hconv2[2*i+1][j][k] ,bitlen);
            for (uint32_t l = 0; l < 5; l++)
            {
                for (uint32_t m = 0; m < 5; m++)
                {
                    share * s_a_tmp_10 = acirc->PutMULGate( s_a_wconv2[2*i][l][m] , s_a_hpool1[i][j+l][k+m] );
                    s_a_hconv2[2*i][j][k] = acirc->PutADDGate( s_a_hconv2[2*i][j][k] , s_a_tmp_10 );
                    share * s_a_tmp_11 = acirc->PutMULGate( s_a_wconv2[2*i+1][l][m] , s_a_hpool1[i][j+l][k+m] );
                    s_a_hconv2[2*i+1][j][k] = acirc->PutADDGate( s_a_hconv2[2*i+1][j][k] , s_a_tmp_11 );
                }
            }
            s_a_hconv2[2*i][j][k] = acirc->PutADDGate( s_a_hconv2[2*i][j][k] , s_a_bconv2[2*i] );
            s_a_hconv2[2*i+1][j][k] = acirc->PutADDGate( s_a_hconv2[2*i+1][j][k] , s_a_bconv2[2*i+1] );
        }
    }
}
for (int _i1 = 0; _i1 < 64 ; _i1++) {
for (int _i4 = 0; _i4 < 14 ; _i4++) {
for (int _i7 = 0; _i7 < 14 ; _i7++) {
s_y_hconv2[_i1][_i4][_i7] = ycirc->PutA2YGate( s_a_hconv2[_i1][_i4][_i7] );
}
}
}
for (uint32_t i = 0; i < 64; i++)
{
    for (uint32_t j = 0; j < 14; j++)
    {
        for (uint32_t k = 0; k < 14; k++)
        {
            uint32_t _tmp_13 = 2147483648 ;
            share * s_y__tmp_13 = ycirc->PutCONSGate( _tmp_13 ,bitlen);
            share * s_y_tmp_12 = ycirc->PutGTGate( s_y_hconv2[i][j][k] , s_y__tmp_13 );
            uint32_t _tmp_14 = 0 ;
            share * s_y__tmp_14 = ycirc->PutCONSGate( _tmp_14 ,bitlen);
            vector<uint32_t> _vec16 = s_y_hconv2[i][j][k]->get_wires();
            _vec16.erase( _vec16.begin(), _vec16.begin() + 8 );
            share *s_y_tmp_15 = create_new_share( _vec16 , ycirc );
            s_y_hconv2[i][j][k] = ycirc->PutMUXGate( s_y__tmp_14 , s_y_tmp_15 , s_y_tmp_12 );
        }
    }
}
auto hpool2 = make_vector<uint32_t>(64, 7, 7);
auto s_y_hpool2 = make_vector<share*>(64, 7, 7);
for (uint32_t i = 0; i < 64; i++)
{
    for (uint32_t j = 0; j < 14; j += 2)
    {
        for (uint32_t k = 0; k < 14; k += 2)
        {
            hpool2[i][j/2][k/2] = 0 ;
            s_y_hpool2[i][j/2][k/2] = ycirc->PutCONSGate( hpool2[i][j/2][k/2] ,bitlen);
            share * s_y_tmp_17 = ycirc->PutGTGate( s_y_hpool2[i][j/2][k/2] , s_y_hconv2[i][j][k] );
            s_y_hpool2[i][j/2][k/2] = ycirc->PutMUXGate( s_y_hpool2[i][j/2][k/2] , s_y_hconv2[i][j][k] , s_y_tmp_17 );
            share * s_y_tmp_18 = ycirc->PutGTGate( s_y_hpool2[i][j/2][k/2] , s_y_hconv2[i][j+1][k] );
            s_y_hpool2[i][j/2][k/2] = ycirc->PutMUXGate( s_y_hpool2[i][j/2][k/2] , s_y_hconv2[i][j+1][k] , s_y_tmp_18 );
            share * s_y_tmp_19 = ycirc->PutGTGate( s_y_hpool2[i][j/2][k/2] , s_y_hconv2[i][j][k+1] );
            s_y_hpool2[i][j/2][k/2] = ycirc->PutMUXGate( s_y_hpool2[i][j/2][k/2] , s_y_hconv2[i][j][k+1] , s_y_tmp_19 );
            share * s_y_tmp_20 = ycirc->PutGTGate( s_y_hpool2[i][j/2][k/2] , s_y_hconv2[i][j+1][k+1] );
            s_y_hpool2[i][j/2][k/2] = ycirc->PutMUXGate( s_y_hpool2[i][j/2][k/2] , s_y_hconv2[i][j+1][k+1] , s_y_tmp_20 );
        }
    }
}
auto hpool2flat = make_vector<uint32_t>(3136);
auto s_y_hpool2flat = make_vector<share*>(3136);
auto s_a_hpool2 = make_vector<share*>(64, 7, 7);
for (int _i1 = 0; _i1 < 64 ; _i1++) {
for (int _i4 = 0; _i4 < 7 ; _i4++) {
for (int _i7 = 0; _i7 < 7 ; _i7++) {
s_a_hpool2[_i1][_i4][_i7] = acirc->PutY2AGate( s_y_hpool2[_i1][_i4][_i7] ,bcirc);
}
}
}
auto s_a_hpool2flat = make_vector<share*>(3136);
for (uint32_t i = 0; i < 64; i++)
{
    for (uint32_t j = 0; j < 7; j++)
    {
        for (uint32_t k = 0; k < 7; k++)
        {
            s_a_hpool2flat[49*i+7*j+k] = create_new_share(s_a_hpool2[i][j][k]->get_wires(), acirc );
        }
    }
}
auto wfc1 = make_vector<uint32_t>(1024, 3136);
auto s_a_wfc1 = make_vector<share*>(1024, 3136);
auto bfc1 = make_vector<uint32_t>(1024);
auto s_a_bfc1 = make_vector<share*>(1024);
for (uint32_t i = 0; i < 1024; i++)
{
    for (uint32_t j = 0; j < 3136; j++)
    {
        if (role == CLIENT) {
            wfc1[i][j] = 460 ;
            s_a_wfc1[i][j] = acirc->PutINGate( wfc1[i][j] ,bitlen,CLIENT);
        } else {
            s_a_wfc1[i][j] = acirc->PutDummyINGate(bitlen);
        }
    }
}
for (uint32_t i = 0; i < 1024; i++)
{
    if (role == CLIENT) {
        bfc1[i] = 66892 ;
        s_a_bfc1[i] = acirc->PutINGate( bfc1[i] ,bitlen,CLIENT);
    } else {
        s_a_bfc1[i] = acirc->PutDummyINGate(bitlen);
    }
}
auto hfc1 = make_vector<uint32_t>(1024);
auto s_a_hfc1 = make_vector<share*>(1024);
for (uint32_t i = 0; i < 1024; i++)
{
    hfc1[i] = 0 ;
    s_a_hfc1[i] = acirc->PutCONSGate( hfc1[i] ,bitlen);
    for (uint32_t j = 0; j < 3136; j++)
    {
        share * s_a_tmp_21 = acirc->PutMULGate( s_a_wfc1[i][j] , s_a_hpool2flat[j] );
        s_a_hfc1[i] = acirc->PutADDGate( s_a_hfc1[i] , s_a_tmp_21 );
    }
    s_a_hfc1[i] = acirc->PutADDGate( s_a_hfc1[i] , s_a_bfc1[i] );
}
auto s_y_hfc1 = make_vector<share*>(1024);
for (int _i1 = 0; _i1 < 1024 ; _i1++) {
s_y_hfc1[_i1] = ycirc->PutA2YGate( s_a_hfc1[_i1] );
}
for (uint32_t i = 0; i < 1024; i++)
{
    uint32_t _tmp_23 = 2147483648 ;
    share * s_y__tmp_23 = ycirc->PutCONSGate( _tmp_23 ,bitlen);
    share * s_y_tmp_22 = ycirc->PutGTGate( s_y_hfc1[i] , s_y__tmp_23 );
    uint32_t _tmp_24 = 0 ;
    share * s_y__tmp_24 = ycirc->PutCONSGate( _tmp_24 ,bitlen);
    s_y_hfc1[i] = ycirc->PutMUXGate( s_y__tmp_24 , s_y_hfc1[i] , s_y_tmp_22 );
}
auto wfc2 = make_vector<uint32_t>(10, 1024);
auto s_y_wfc2 = make_vector<share*>(10, 1024);
auto bfc2 = make_vector<uint32_t>(10);
auto s_y_bfc2 = make_vector<share*>(10);
auto s_a_wfc2 = make_vector<share*>(10, 1024);
for (uint32_t i = 0; i < 10; i++)
{
    for (uint32_t j = 0; j < 1024; j++)
    {
        if (role == CLIENT) {
            wfc2[i][j] = 460 ;
            s_a_wfc2[i][j] = acirc->PutINGate( wfc2[i][j] ,bitlen,CLIENT);
        } else {
            s_a_wfc2[i][j] = acirc->PutDummyINGate(bitlen);
        }
    }
}
auto s_a_bfc2 = make_vector<share*>(10);
for (uint32_t i = 0; i < 10; i++)
{
    if (role == CLIENT) {
        bfc2[i] = 66892 ;
        s_a_bfc2[i] = acirc->PutINGate( bfc2[i] ,bitlen,CLIENT);
    } else {
        s_a_bfc2[i] = acirc->PutDummyINGate(bitlen);
    }
}
auto yconv = make_vector<uint32_t>(10);
auto s_a_yconv = make_vector<share*>(10);
for (int _i1 = 0; _i1 < 1024 ; _i1++) {
s_a_hfc1[_i1] = acirc->PutY2AGate( s_y_hfc1[_i1] ,bcirc);
}
for (uint32_t i = 0; i < 10; i++)
{
    yconv[i] = 0 ;
    s_a_yconv[i] = acirc->PutCONSGate( yconv[i] ,bitlen);
    for (uint32_t j = 0; j < 1024; j++)
    {
        share * s_a_tmp_25 = acirc->PutMULGate( s_a_wfc2[i][j] , s_a_hfc1[j] );
        s_a_yconv[i] = acirc->PutADDGate( s_a_yconv[i] , s_a_tmp_25 );
    }
    s_a_yconv[i] = acirc->PutADDGate( s_a_yconv[i] , s_a_bfc2[i] );
}
uint32_t max;
share *s_a_max;
max = 0 ;
s_a_max = acirc->PutCONSGate( max ,bitlen);
auto s_y_yconv = make_vector<share*>(10);
for (int _i1 = 0; _i1 < 10 ; _i1++) {
s_y_yconv[_i1] = ycirc->PutA2YGate( s_a_yconv[_i1] );
}
share *s_y_max = ycirc->PutA2YGate( s_a_max );
for (uint32_t i = 0; i < 10; i++)
{
    share * s_y_tmp_26 = ycirc->PutGTGate( s_y_yconv[i] , s_y_max );
    s_y_max = ycirc->PutMUXGate( s_y_yconv[i] , s_y_max , s_y_tmp_26 );
}
uint32_t _tmp_29 = 47 ;
share * s_y__tmp_29 = ycirc->PutCONSGate( _tmp_29 ,bitlen);
vector<uint32_t> _vec30 = s_y__tmp_29->get_wires();
_vec30.erase( _vec30.begin(), _vec30.begin() + 2 );
share *s_y_tmp_28 = create_new_share( _vec30 , ycirc );
share * s_y_tmp_27 = ycirc->PutOUTGate( s_y_tmp_28 , ALL);
party->ExecCircuit();
uint32_t _output = s_y_tmp_27->get_clear_value<uint32_t>();
