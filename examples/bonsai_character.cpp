ABYParty *party = new ABYParty(role, address, port, seclvl, bitlen, nthreads, mt_alg, 100000000);
vector<Sharing*>& sharings = party->GetSharings();
Circuit* ycirc = sharings[S_YAO]->GetCircuitBuildRoutine();
Circuit* acirc = sharings[S_ARITH]->GetCircuitBuildRoutine();
Circuit* bcirc = sharings[S_BOOL]->GetCircuitBuildRoutine();
auto testX = make_vector<uint32_t>(400);
auto s_a_testX = make_vector<share*>(400);
for (uint32_t i = 0; i < 400; i++)
{
    if (role == SERVER) {
        testX[i] = 50 ;
        s_a_testX[i] = acirc->PutINGate( testX[i] ,bitlen,SERVER);
    } else {
        s_a_testX[i] = acirc->PutDummyINGate(bitlen);
    }
}
auto Z = make_vector<uint32_t>(5, 400);
auto s_a_Z = make_vector<share*>(5, 400);
for (uint32_t i = 0; i < 5; i++)
{
    for (uint32_t j = 0; j < 400; j++)
    {
        if (role == CLIENT) {
            Z[i][j] = 1000 ;
            s_a_Z[i][j] = acirc->PutINGate( Z[i][j] ,bitlen,CLIENT);
        } else {
            s_a_Z[i][j] = acirc->PutDummyINGate(bitlen);
        }
    }
}
auto ZX = make_vector<uint32_t>(5);
auto s_a_ZX = make_vector<share*>(5);
for (uint32_t i = 0; i < 5; i++)
{
    ZX[i] = 0 ;
    s_a_ZX[i] = acirc->PutCONSGate( ZX[i] ,bitlen);
    for (uint32_t j = 0; j < 400; j++)
    {
        share * s_a_tmp_0 = acirc->PutMULGate( s_a_testX[i] , s_a_Z[i][j] );
        s_a_ZX[i] = acirc->PutADDGate( s_a_ZX[i] , s_a_tmp_0 );
    }
}
uint32_t Score;
share *s_a_Score;
Score = 0 ;
s_a_Score = acirc->PutCONSGate( Score ,bitlen);
uint32_t curr;
share *s_a_curr;
curr = 0 ;
s_a_curr = acirc->PutCONSGate( curr ,bitlen);
uint32_t VZX;
share *s_a_VZX;
VZX = 0 ;
s_a_VZX = acirc->PutCONSGate( VZX ,bitlen);
auto V = make_vector<uint32_t>(5);
auto s_a_V = make_vector<share*>(5);
for (uint32_t i = 0; i < 5; i++)
{
    if (role == CLIENT) {
        V[i] = 150 ;
        s_a_V[i] = acirc->PutINGate( V[i] ,bitlen,CLIENT);
    } else {
        s_a_V[i] = acirc->PutDummyINGate(bitlen);
    }
}
for (uint32_t i = 0; i < 5; i++)
{
    share * s_a_tmp_1 = acirc->PutMULGate( s_a_V[i] , s_a_ZX[i] );
    s_a_VZX = acirc->PutADDGate( s_a_VZX , s_a_tmp_1 );
}
share *s_y_VZX = ycirc->PutA2YGate( s_a_VZX );
uint32_t _tmp_3 = 100 ;
share * s_y__tmp_3 = ycirc->PutCONSGate( _tmp_3 ,bitlen);
share * s_y_tmp_2 = ycirc->PutGTGate( s_y_VZX , s_y__tmp_3 );
uint32_t _tmp_4 = 100 ;
share * s_y__tmp_4 = ycirc->PutCONSGate( _tmp_4 ,bitlen);
uint32_t _tmp_7 = 10 ;
share * s_y__tmp_7 = ycirc->PutCONSGate( _tmp_7 ,bitlen);
share * s_y_tmp_6 = ycirc->PutGTGate( s_y_VZX , s_y__tmp_7 );
uint32_t _tmp_8 = 10 ;
share * s_y__tmp_8 = ycirc->PutCONSGate( _tmp_8 ,bitlen);
share * s_y_tmp_5 = ycirc->PutMUXGate( s_y_VZX , s_y__tmp_8 , s_y_tmp_6 );
s_y_VZX = ycirc->PutMUXGate( s_y__tmp_4 , s_y_tmp_5 , s_y_tmp_2 );
uint32_t WZX;
share *s_y_WZX;
WZX = 0 ;
s_y_WZX = ycirc->PutCONSGate( WZX ,bitlen);
auto W = make_vector<uint32_t>(5);
auto s_y_W = make_vector<share*>(5);
auto s_a_W = make_vector<share*>(5);
for (uint32_t i = 0; i < 5; i++)
{
    if (role == CLIENT) {
        W[i] = 150 ;
        s_a_W[i] = acirc->PutINGate( W[i] ,bitlen,CLIENT);
    } else {
        s_a_W[i] = acirc->PutDummyINGate(bitlen);
    }
}
share *s_a_WZX = acirc->PutY2AGate( s_y_WZX ,bcirc);
for (uint32_t i = 0; i < 5; i++)
{
    share * s_a_tmp_9 = acirc->PutMULGate( s_a_W[i] , s_a_ZX[i] );
    s_a_WZX = acirc->PutADDGate( s_a_WZX , s_a_tmp_9 );
}
s_a_VZX = acirc->PutY2AGate( s_y_VZX ,bcirc);
share * s_a_tmp_10 = acirc->PutMULGate( s_a_VZX , s_a_WZX );
s_a_Score = acirc->PutADDGate( s_a_Score , s_a_tmp_10 );
uint32_t TZX;
share *s_a_TZX;
TZX = 0 ;
s_a_TZX = acirc->PutCONSGate( TZX ,bitlen);
auto T = make_vector<uint32_t>(5);
auto s_a_T = make_vector<share*>(5);
for (uint32_t i = 0; i < 5; i++)
{
    if (role == CLIENT) {
        T[i] = 150 ;
        s_a_T[i] = acirc->PutINGate( T[i] ,bitlen,CLIENT);
    } else {
        s_a_T[i] = acirc->PutDummyINGate(bitlen);
    }
}
for (uint32_t i = 0; i < 5; i++)
{
    share * s_a_tmp_11 = acirc->PutMULGate( s_a_T[i] , s_a_ZX[i] );
    s_a_TZX = acirc->PutADDGate( s_a_TZX , s_a_tmp_11 );
}
share *s_y_TZX = ycirc->PutA2YGate( s_a_TZX );
uint32_t _tmp_13 = 100 ;
share * s_y__tmp_13 = ycirc->PutCONSGate( _tmp_13 ,bitlen);
share * s_y_tmp_12 = ycirc->PutGTGate( s_y_TZX , s_y__tmp_13 );
uint32_t _tmp_16 = 2 ;
share * s_a__tmp_16 = acirc->PutCONSGate( _tmp_16 ,bitlen);
share * s_a_tmp_15 = acirc->PutMULGate( s_a__tmp_16 , s_a_curr );
uint32_t _tmp_17 = 1 ;
share * s_a__tmp_17 = acirc->PutCONSGate( _tmp_17 ,bitlen);
share * s_a_tmp_14 = acirc->PutADDGate( s_a_tmp_15 , s_a__tmp_17 );
uint32_t _tmp_20 = 2 ;
share * s_a__tmp_20 = acirc->PutCONSGate( _tmp_20 ,bitlen);
share * s_a_tmp_19 = acirc->PutMULGate( s_a__tmp_20 , s_a_curr );
uint32_t _tmp_21 = 2 ;
share * s_a__tmp_21 = acirc->PutCONSGate( _tmp_21 ,bitlen);
share * s_a_tmp_18 = acirc->PutADDGate( s_a_tmp_19 , s_a__tmp_21 );
share *s_y_tmp_14 = ycirc->PutA2YGate( s_a_tmp_14 );
share *s_y_tmp_18 = ycirc->PutA2YGate( s_a_tmp_18 );
share * s_y_curr = ycirc->PutMUXGate( s_y_tmp_14 , s_y_tmp_18 , s_y_tmp_12 );
auto V1 = make_vector<uint32_t>(5);
auto s_y_V1 = make_vector<share*>(5);
auto s_a_V1 = make_vector<share*>(5);
for (uint32_t i = 0; i < 5; i++)
{
    if (role == CLIENT) {
        V1[i] = 150 ;
        s_a_V1[i] = acirc->PutINGate( V1[i] ,bitlen,CLIENT);
    } else {
        s_a_V1[i] = acirc->PutDummyINGate(bitlen);
    }
}
auto V2 = make_vector<uint32_t>(5);
auto s_a_V2 = make_vector<share*>(5);
for (uint32_t i = 0; i < 5; i++)
{
    if (role == CLIENT) {
        V2[i] = 150 ;
        s_a_V2[i] = acirc->PutINGate( V2[i] ,bitlen,CLIENT);
    } else {
        s_a_V2[i] = acirc->PutDummyINGate(bitlen);
    }
}
VZX = 0 ;
s_a_VZX = acirc->PutCONSGate( VZX ,bitlen);
uint32_t tmp;
share *s_a_tmp;
auto s_y_V2 = make_vector<share*>(5);
for (int _i1 = 0; _i1 < 5 ; _i1++) {
s_y_V2[_i1] = ycirc->PutA2YGate( s_a_V2[_i1] );
}
for (int _i1 = 0; _i1 < 5 ; _i1++) {
s_y_V1[_i1] = ycirc->PutA2YGate( s_a_V1[_i1] );
}
share * s_y_tmp ;
s_y_VZX = ycirc->PutA2YGate( s_a_VZX );
auto s_y_ZX = make_vector<share*>(5);
for (int _i1 = 0; _i1 < 5 ; _i1++) {
s_y_ZX[_i1] = ycirc->PutA2YGate( s_a_ZX[_i1] );
}
for (uint32_t i = 0; i < 5; i++)
{
    uint32_t _tmp_23 = 1 ;
    share * s_y__tmp_23 = ycirc->PutCONSGate( _tmp_23 ,bitlen);
    share * s_y_tmp_22 = ycirc->PutGTGate( s_y_curr , s_y__tmp_23 );
    s_y_tmp = ycirc->PutMUXGate( s_y_V2[i] , s_y_V1[i] , s_y_tmp_22 );
    share * s_y_tmp_24 = ycirc->PutMULGate( s_y_tmp , s_y_ZX[i] );
    s_y_VZX = ycirc->PutADDGate( s_y_VZX , s_y_tmp_24 );
}
uint32_t _tmp_26 = 100 ;
share * s_y__tmp_26 = ycirc->PutCONSGate( _tmp_26 ,bitlen);
share * s_y_tmp_25 = ycirc->PutGTGate( s_y_VZX , s_y__tmp_26 );
uint32_t _tmp_27 = 100 ;
share * s_y__tmp_27 = ycirc->PutCONSGate( _tmp_27 ,bitlen);
uint32_t _tmp_30 = 10 ;
share * s_y__tmp_30 = ycirc->PutCONSGate( _tmp_30 ,bitlen);
share * s_y_tmp_29 = ycirc->PutGTGate( s_y_VZX , s_y__tmp_30 );
uint32_t _tmp_31 = 10 ;
share * s_y__tmp_31 = ycirc->PutCONSGate( _tmp_31 ,bitlen);
share * s_y_tmp_28 = ycirc->PutMUXGate( s_y_VZX , s_y__tmp_31 , s_y_tmp_29 );
s_y_VZX = ycirc->PutMUXGate( s_y__tmp_27 , s_y_tmp_28 , s_y_tmp_25 );
WZX = 0 ;
s_y_WZX = ycirc->PutCONSGate( WZX ,bitlen);
auto W1 = make_vector<uint32_t>(5);
auto s_y_W1 = make_vector<share*>(5);
auto s_a_W1 = make_vector<share*>(5);
for (uint32_t i = 0; i < 5; i++)
{
    if (role == CLIENT) {
        W1[i] = 150 ;
        s_a_W1[i] = acirc->PutINGate( W1[i] ,bitlen,CLIENT);
    } else {
        s_a_W1[i] = acirc->PutDummyINGate(bitlen);
    }
}
auto W2 = make_vector<uint32_t>(5);
auto s_a_W2 = make_vector<share*>(5);
for (uint32_t i = 0; i < 5; i++)
{
    if (role == CLIENT) {
        W2[i] = 150 ;
        s_a_W2[i] = acirc->PutINGate( W2[i] ,bitlen,CLIENT);
    } else {
        s_a_W2[i] = acirc->PutDummyINGate(bitlen);
    }
}
auto s_y_W2 = make_vector<share*>(5);
for (int _i1 = 0; _i1 < 5 ; _i1++) {
s_y_W2[_i1] = ycirc->PutA2YGate( s_a_W2[_i1] );
}
for (int _i1 = 0; _i1 < 5 ; _i1++) {
s_y_W1[_i1] = ycirc->PutA2YGate( s_a_W1[_i1] );
}
for (uint32_t i = 0; i < 5; i++)
{
    uint32_t _tmp_33 = 1 ;
    share * s_y__tmp_33 = ycirc->PutCONSGate( _tmp_33 ,bitlen);
    share * s_y_tmp_32 = ycirc->PutGTGate( s_y_curr , s_y__tmp_33 );
    s_y_tmp = ycirc->PutMUXGate( s_y_W2[i] , s_y_W1[i] , s_y_tmp_32 );
    share * s_y_tmp_34 = ycirc->PutMULGate( s_y_tmp , s_y_ZX[i] );
    s_y_WZX = ycirc->PutADDGate( s_y_WZX , s_y_tmp_34 );
}
s_a_VZX = acirc->PutY2AGate( s_y_VZX ,bcirc);
s_a_WZX = acirc->PutY2AGate( s_y_WZX ,bcirc);
share * s_a_tmp_35 = acirc->PutMULGate( s_a_VZX , s_a_WZX );
s_a_Score = acirc->PutADDGate( s_a_Score , s_a_tmp_35 );
uint32_t out;
share *s_a_out;
share *s_y_Score = ycirc->PutA2YGate( s_a_Score );
uint32_t _tmp_37 = 100 ;
share * s_y__tmp_37 = ycirc->PutCONSGate( _tmp_37 ,bitlen);
share * s_y_tmp_36 = ycirc->PutGTGate( s_y_Score , s_y__tmp_37 );
uint32_t _tmp_38 = 1 ;
share * s_y__tmp_38 = ycirc->PutCONSGate( _tmp_38 ,bitlen);
uint32_t _tmp_39 = 0 ;
share * s_y__tmp_39 = ycirc->PutCONSGate( _tmp_39 ,bitlen);
share * s_y_out = ycirc->PutMUXGate( s_y__tmp_38 , s_y__tmp_39 , s_y_tmp_36 );
share * s_y_tmp_40 = ycirc->PutOUTGate( s_y_out , ALL);
party->ExecCircuit();
uint32_t _output = s_y_tmp_40->get_clear_value<uint32_t>();