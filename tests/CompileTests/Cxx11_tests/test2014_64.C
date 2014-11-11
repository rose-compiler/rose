void ApplyScale(const vector<int>& v) const
{
   for_each(v.begin(), v.end(), 
      [this](int n) { cout << n * _scale << endl; });
}

