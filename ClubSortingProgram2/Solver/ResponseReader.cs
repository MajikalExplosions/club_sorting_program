﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ClubSortingProgram2.Solver
{
    public class ResponseReader
    {
        public static string[] GetHeaders(Response r)
        {
            return r.GetRow(0);
        }

        public static string[] GetUnique(string[] arr)
        {
            List<string> unique = new List<string>();
            foreach (string s in arr)
            {
                if (!unique.Contains(s)) unique.Add(s);
            }
            return unique.ToArray();
        }

        public static string[] GetColumnWithHeader(Response r, string header)
        {
            int index = Array.IndexOf(GetHeaders(r), header);
            if (index == -1)
            {
                MainScreen.Instance.AlertError("Header " + header + " not found in file.");
                return null;
            }
            return r.GetColumn(index);
        }
    }
}