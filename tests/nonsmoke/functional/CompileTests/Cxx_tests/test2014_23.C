// ROSE bug from Chombo application code.

IntVectSet& IntVectSet::operator|=(const IntVectSet& ivs)
{
  if (m_isdense)
    {
      if (ivs.m_isdense)
        {
       // DQ (3/27/2014): mixBox is not name qualified.
          if (CH_XD::minBox(m_dense.box(), ivs.m_dense.box()).numPts() <=
              std::max(m_dense.box().numPts(), ivs.m_dense.box().numPts()))
            {
              m_dense |= ivs.m_dense;
              return *this;
            }
          ivs.convert();
        }
      convert();
    }
  else if (ivs.m_isdense)
    {
      ivs.convert();
    }
  m_ivs |= ivs.m_ivs;
  return *this;
}
