ABYParty *party = new ABYParty(role, address, port, seclvl, bitlen, nthreads, mt_alg, 100000000);
vector<Sharing*>& sharings = party->GetSharings();
Circuit* ycirc = sharings[S_YAO]->GetCircuitBuildRoutine();
Circuit* acirc = sharings[S_ARITH]->GetCircuitBuildRoutine();
Circuit* bcirc = sharings[S_BOOL]->GetCircuitBuildRoutine();
auto W = make_vector<uint32_t>(6);
auto s_a_W = make_vector<share*>(6);
auto x = make_vector<uint32_t>(6);
auto s_a_x = make_vector<share*>(6);
auto B = make_vector<uint32_t>(6);
auto s_a_B = make_vector<share*>(6);
uint32_t out;
share *s_a_out;
for (uint32_t i = 0; i < 6; i++)
{
    if (role == SERVER) {
        W[i] = 25 ;
        s_a_W[i] = acirc->PutINGate( W[i] ,bitlen,SERVER);
    } else {
        s_a_W[i] = acirc->PutDummyINGate(bitlen);
    }
}
for (uint32_t i = 0; i < 6; i++)
{
    if (role == CLIENT) {
        x[i] = 40 ;
        s_a_x[i] = acirc->PutINGate( x[i] ,bitlen,CLIENT);
    } else {
        s_a_x[i] = acirc->PutDummyINGate(bitlen);
    }
}
auto s_y_W = make_vector<share*>(6);
for (int _i1 = 0; _i1 < 6 ; _i1++) {
s_y_W[_i1] = ycirc->PutA2YGate( s_a_W[_i1] );
}
auto s_y_x = make_vector<share*>(6);
for (int _i1 = 0; _i1 < 6 ; _i1++) {
s_y_x[_i1] = ycirc->PutA2YGate( s_a_x[_i1] );
}
auto s_y_B = make_vector<share*>(6);
for (uint32_t i = 0; i < 6; i++)
{
    share * s_y_tmp_0 = ycirc->PutGTGate( s_y_W[i] , s_y_x[i] );
    uint32_t _tmp_1 = 0 ;
    share * s_y__tmp_1 = ycirc->PutCONSGate( _tmp_1 ,bitlen);
    uint32_t _tmp_2 = 1 ;
    share * s_y__tmp_2 = ycirc->PutCONSGate( _tmp_2 ,bitlen);
    s_y_B[i] = ycirc->PutMUXGate( s_y__tmp_1 , s_y__tmp_2 , s_y_tmp_0 );
}
uint32_t _tmp_5 = 1 ;
share * s_a__tmp_5 = acirc->PutCONSGate( _tmp_5 ,bitlen);
for (int _i1 = 0; _i1 < 6 ; _i1++) {
s_a_B[_i1] = acirc->PutY2AGate( s_y_B[_i1] ,bcirc);
}
share * s_a_tmp_4 = acirc->PutSUBGate( s_a__tmp_5 , s_a_B[0] );
uint32_t _tmp_9 = 1 ;
share * s_a__tmp_9 = acirc->PutCONSGate( _tmp_9 ,bitlen);
share * s_a_tmp_8 = acirc->PutSUBGate( s_a__tmp_9 , s_a_B[1] );
uint32_t _tmp_13 = 1 ;
share * s_a__tmp_13 = acirc->PutCONSGate( _tmp_13 ,bitlen);
share * s_a_tmp_12 = acirc->PutSUBGate( s_a__tmp_13 , s_a_B[3] );
share * s_a_tmp_11 = acirc->PutMULGate( s_a_tmp_12 , s_a_B[5] );
share * s_a_tmp_10 = acirc->PutADDGate( s_a_B[3] , s_a_tmp_11 );
share * s_a_tmp_7 = acirc->PutMULGate( s_a_tmp_8 , s_a_tmp_10 );
share * s_a_tmp_6 = acirc->PutADDGate( s_a_B[1] , s_a_tmp_7 );
share * s_a_tmp_3 = acirc->PutMULGate( s_a_tmp_4 , s_a_tmp_6 );
uint32_t _tmp_18 = 1 ;
share * s_a__tmp_18 = acirc->PutCONSGate( _tmp_18 ,bitlen);
share * s_a_tmp_17 = acirc->PutSUBGate( s_a__tmp_18 , s_a_B[2] );
share * s_a_tmp_16 = acirc->PutMULGate( s_a_tmp_17 , s_a_B[4] );
share * s_a_tmp_15 = acirc->PutADDGate( s_a_B[2] , s_a_tmp_16 );
share * s_a_tmp_14 = acirc->PutMULGate( s_a_B[0] , s_a_tmp_15 );
s_a_out = acirc->PutADDGate( s_a_tmp_3 , s_a_tmp_14 );
share * s_a_tmp_19 = acirc->PutOUTGate( s_a_out , ALL);
party->ExecCircuit();
uint32_t _output = s_a_tmp_19->get_clear_value<uint32_t>();
