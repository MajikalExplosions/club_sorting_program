using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ClubSortingProgram2.Solver
{
    public class SolverException : Exception
    {
        public readonly int Code;
        public readonly string Description;

        public SolverException() : this(0, "Unexpected solver error")
        {
        }

        public SolverException(int code, string desc) : base(string.Format("({0,3:D3}) {1}", code, desc))
        {
            Code = code;
            Description = desc;
        }
    }
}