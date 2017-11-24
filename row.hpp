#pragma once 




class Row
{
    public:
       Row(ResultSetSPtr pRes):resultSet(pRes)
       {

       }


    private:
       ResultSetSPtr  resultSet; 
}; 
