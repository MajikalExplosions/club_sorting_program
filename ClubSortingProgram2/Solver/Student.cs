using System.Collections.Generic;

namespace ClubSortingProgram2.Solver
{
    public class Student
    {
        public readonly string Name;
        public Club[] Assignments { get; private set; }
        public List<Club>[] Requests { get; private set; }

        public Student(string name)
        {
            Name = name;
            Assignments = new Club[Solver.Settings.Weeks];
            Requests = new List<Club>[Solver.Settings.Weeks];
        }

        public void SetRequests(int week, List<Club> requests)
        {
            if (week < 0 || week > Requests.Length - 1)
            {
                MainScreen.Instance.AlertError(week.ToString() + "out of bounds; there are only " + Requests.Length + "weeks.");
                return;
            }
            if (requests.Count > Solver.Settings.MaxRequests)
            {
                MainScreen.Instance.AlertError(Name + " has too many requests: " + requests.Count + ".");
                return;
            }

            if (Requests[week].Count != 0) Requests[week].Clear();
            foreach (Club c in requests) Requests[week].Add(c);
        }
    }
}