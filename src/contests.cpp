

#include "Contests.h"
#include "GTimer.h"
extern GTimer GT;
extern CompetitionSaved competition;
//extern Contests mycompetition;

void Contests::calculate_scores()
{ 
    
    Serial.printf(" Rank Table =================>\n");
    GT.clear_draw_list_scores();
    for (int round = 1; round < MAX_SELECTION_POINTER_ROUND; round++)
    {
        // 4.16 group will only be =1
        Serial.printf(" Ronda===========: %d\n",round);
        for (int group = 1; group < MAX_SELECTION_POINTER_GROUP; group++)
        {
            Serial.printf("     Grupo===========: %d\n",group);
            uint32_t max_raw = 0; //max raw in group/round

            for (int pilot = 1; pilot < MAX_SELECTION_POINTER_PILOT; pilot++)
            {
                if (GT.draw_data[round][pilot].group_no == group)
                {
                    if (GT.draw_data[round][pilot].raw_score > max_raw)
                    {
                        max_raw = GT.draw_data[round][pilot].raw_score;
                                            }
                }
            }
            Serial.printf(" MAx Raw : %d",max_raw);
            if (max_raw == 0)
            {
                max_raw = 1;
            }

            for (int pilot = 1; pilot < MAX_SELECTION_POINTER_PILOT; pilot++) //FAI_points
            {
                if (GT.draw_data[round][pilot].group_no == group)
                {
                    if (max_raw > 0)
                    {
                        draw_list[round][pilot].FAI_score = (uint32_t)((1000 * GT.draw_data[round][pilot].raw_score) / max_raw);
                        Serial.printf("Pilot : %d FAI_A= ", pilot);
                        Serial.println(draw_list[round][pilot].FAI_score);
                        if (draw_list[round][pilot].FAI_score > GT.draw_data[round][pilot].penalty_pts)
                        {
                            draw_list[round][pilot].FAI_score = draw_list[round][pilot].FAI_score - GT.draw_data[round][pilot].penalty_pts;
                            
                        }
                        else
                        {
                            draw_list[round][pilot].FAI_score = 0;
                        }
                    }
                }
            }
            //Round_Rank
            Serial.printf("Rank calcs\n");
            for (uint32_t pilot_rank = 1; pilot_rank <= head.number_of_pilots; pilot_rank++)
            {
                Serial.printf("       pilot_rank  %d\n",pilot_rank);
                // for (int group = 1; group < MAX_SELECTION_POINTER_GROUP; group++)
                // {
                    uint32_t max_FAI = 0; //max FAI value in those not yet ranked

                    for (int pilot = 1; pilot < MAX_SELECTION_POINTER_PILOT; pilot++)
                    {
                        if (GT.draw_data[round][pilot].group_no == group)
                        {
                            if (draw_list[round][pilot].round_rank == 0) /// not evaluated yet
                            {
                                if (draw_list[round][pilot].FAI_score >= max_FAI)
                                {
                                    max_FAI = draw_list[round][pilot].FAI_score;
                                    GT.rank_list[round][group][pilot_rank] = pilot;
                                    Serial.printf(" round: %d rank %d Piloto %d \n",round, pilot_rank, pilot);

                                    max_FAI++;
                                }
                            }
                        }
                    }

                    if (max_FAI > 0)
                    {
                        draw_list[round][GT.rank_list[round][group][pilot_rank]].round_rank = pilot_rank;
                        Serial.printf(" round: %d rank %d Piloto %d \n",round, pilot_rank, GT.rank_list[round][group][pilot_rank]);
                    }
                    else
                    {
                        Serial.print("BlanK/ ");
                    }
                // }
            }
        }
    }

    int final = 0;
    int groupk =0;
    for (int pilot = 1; pilot < MAX_SELECTION_POINTER_PILOT; pilot++) //Competition scores
    {
        int min_FAI = 1000;
        for (int round = 1; round < MAX_SELECTION_POINTER_ROUND; round++)
        {
            draw_list[final][pilot].FAI_score += draw_list[round][pilot].FAI_score;
           // draw_list[final][pilot].penalty += draw_list[round][pilot].penalty;
            if (draw_list[round][pilot].FAI_score < min_FAI)
            {
                min_FAI = draw_list[round][pilot].FAI_score;
                draw_list[final][pilot].discarded_round = round;
            }
        }

        if (GT.stored_round_no > 4)
        {
            int discarded_round = draw_list[final][pilot].discarded_round;
            draw_list[final][pilot].FAI_score = draw_list[final][pilot].FAI_score - draw_list[discarded_round][pilot].FAI_score;
            //draw_list[final][pilot].FAI_score += draw_list[discarded_round][pilot].penalty;
        }
    }
    //final rank
    for (uint32_t pilot_rank = 1; pilot_rank < MAX_SELECTION_POINTER_PILOT; pilot_rank++)
    {
        uint32_t max_FAI = 0;
        for (int pilot = 1; pilot < MAX_SELECTION_POINTER_PILOT; pilot++)
        {
            if (draw_list[final][pilot].round_rank == 0)
            {
                if (draw_list[final][pilot].FAI_score > max_FAI)
                {
                    max_FAI = draw_list[final][pilot].FAI_score;
                    GT.rank_list[final][groupk][pilot_rank] = pilot;
                }
            }
        }
        draw_list[final][GT.rank_list[final][groupk][pilot_rank]].round_rank = pilot_rank;
    }
}

void Contests::read_rank_list()
{

    uint32_t number_of_rounds;
    for (int roundi = 0; roundi < (GT.mycompetition.head.number_of_rounds + 1); roundi++)
    {
        //GT.rank_list[roundi][pilotj]=0
        for (int pilotj = 1; pilotj < (GT.mycompetition.head.number_of_pilots + 1); pilotj++)
        {
            for (int groupk = 1; groupk < (GT.mycompetition.head.number_of_groups + 1); groupk++)
            {
                GT.rank_list[roundi][groupk][GT.mycompetition.draw_list[roundi][pilotj].round_rank] = pilotj;
            }
        }
    }
}
void Contests::print_contest()
{

    Serial.print("Contest Head :  ___________________________");
    Serial.print(" contestNum: ");
    Serial.println(head.contestNum);
    Serial.print("number_of_rounds: ");
    Serial.print(head.number_of_rounds);
    Serial.print(" number_of_groups: ");
    Serial.print(head.number_of_groups);
    Serial.print(" number_of_pilots: ");
    Serial.println(head.number_of_pilots);
    Serial.print("Flight duration ");
    Serial.println(head.flt_duration);
    
    Serial.print(" draw_locked: ");
    Serial.print(head.draw_locked);
    Serial.print(" landing_millis_allowance: ");
    Serial.println(head.landing_millis_allowance);
    Serial.print("Record Number ");
    Serial.println(GT.stored_record_num);
    Serial.print("Round Number");
    Serial.println(GT.stored_round_no);
    Serial.print("Group Number ");
    Serial.println(GT.stored_group_no);

    // Serial.print("Pilot_0_id: ");
    // Serial.println(GT.serialnum_contest[0]);
    // Serial.print("Pilot_1_id: ");
    // Serial.println(GT.serialnum_contest[1]);
    // Serial.print("Pilot_2_id: ");
    // Serial.println(GT.serialnum_contest[2]);
    // Serial.print("Pilot_3_id: ");
    // Serial.println(GT.serialnum_contest[3]);
    // Serial.println("Contest Drawlist :  ___________________________");
    // for (int i = 0; i < MAX_SELECTION_POINTER_ROUND; i++)
    // {
    //     Serial.println();
    //     Serial.printf("Round #%d     ############# \n", i);
    //     Serial.println(i);

    //     for (int j = 1; j < MAX_SELECTION_POINTER_PILOT; j++)
    //     {
    //         if ((i <= GT.mycompetition.head.number_of_rounds) && (j <= GT.mycompetition.head.number_of_pilots))
    //         {
    //                 Serial.print("  Pilot #:  ");
    //                 Serial.print(j);
    //                 Serial.print("  pilot_id: ");
    //                 Serial.print(draw_list[i][j].pilot_id);
    //                 Serial.print("  FAI_score: ");
    //                 Serial.print(draw_list[i][j].FAI_score);
    //                 Serial.print("  round_rank: ");
    //                 Serial.print(draw_list[i][j].round_rank);
    //                 Serial.print("  penalty: ");
    //                 Serial.print(draw_list[i][j].penalty);
    //                 Serial.print("  discarded_round: ");
    //                 Serial.println(draw_list[i][j].discarded_round);
    //         }
    //     }
    // }
}

void Contests::close_competition()
{
    calculate_scores();
    GT.stored_competition_closed = 1;
    draw_list[0][0].pilot_id = COMPETITION_CLOSED;
    bool exito = competition.save_competition(&GT.mycompetition);
    // Serial.printf("Competition %d SAVED in flash======================\n", GT.stored_competition_no);
}